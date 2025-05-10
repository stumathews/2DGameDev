#include "pch.h"
#include "Enemy.h"
#include <memory>
#include <utility>
#include "character/DirectionUtils.h"
#include "GameDataManager.h"
#include "GameObjectMoveStrategy.h"
#include "Player.h"
#include "EnemyMovedEvent.h"
#include "character/Movement.h"
#include "PlayerCollidedWithEnemyEvent.h"
#include "EventNumber.h"
#include "GameObjectEventFactory.h"
#include "character/MovementAtSpeed.h"
#include "geometry/SideUtils.h"
#include "Room.h"
#include "SDLCollisionDetection.h"
#include "file/SettingsManager.h"
#include "objects/GameObjectFactory.h"

Enemy::Enemy(const std::string& name,
             const std::string& type,
             const gamelib::Coordinate<int> position,
             const bool visible,
             const std::shared_ptr<Room>& startRoom,
             std::shared_ptr<gamelib::AnimatedSprite> sprite,
             const gamelib::Direction startingDirection,
             std::shared_ptr<const Level> level,
             std::shared_ptr<gamelib::IGameObjectMoveStrategy> enemyMoveStrategy)
	: Npc(name, type, position, visible, std::move(sprite), std::move(enemyMoveStrategy)),
	  CurrentLevel(std::move(level))
{
	SetNpcDirection(startingDirection);

	CurrentRoom = std::make_shared<RoomInfo>(startRoom);
	
	ConfigureEnemyBehavior();
}

void Enemy::ConfigureEnemyBehavior()
{
	// Set up possible states enemy can be in
	upState = gamelib::FSMState("Up", DoLookForPlayer());
	downState = gamelib::FSMState("Down", DoLookForPlayer());
	leftState = gamelib::FSMState("Left", DoLookForPlayer());
	rightState = gamelib::FSMState("Right", DoLookForPlayer());
	hitWallState = gamelib::FSMState("Invalid", gamelib::FSMState::NoUpdate,
		[&] { SwapCurrentDirection(); });

	// Set the state machine states
	invalidMoveTransition = gamelib::FSMTransition([&]()-> bool { return !isValidMove; },
		[&]()-> gamelib::FSMState* { return &hitWallState; });

	// Set up conditions for state transitions
	onUpDirection = gamelib::FSMTransition(IfMovedInDirection(gamelib::Direction::Up),
		[&]()-> gamelib::FSMState* { return &upState; });
	onDownDirection = gamelib::FSMTransition(IfMovedInDirection(gamelib::Direction::Down),
		[&]()-> gamelib::FSMState* { return &downState; });
	onLeftDirection = gamelib::FSMTransition(IfMovedInDirection(gamelib::Direction::Left),
		[&]()-> gamelib::FSMState* { return &leftState; });
	onRightDirection = gamelib::FSMTransition(IfMovedInDirection(gamelib::Direction::Right),
		[&]()-> gamelib::FSMState* { return &rightState; });

	// Configure valid transitions
	upState.Transitions = { onDownDirection, onLeftDirection, onRightDirection, invalidMoveTransition };
	downState.Transitions = { onUpDirection, onLeftDirection, onRightDirection, invalidMoveTransition };
	leftState.Transitions = { onUpDirection, onDownDirection, onRightDirection, invalidMoveTransition };
	rightState.Transitions = { onUpDirection, onDownDirection, onLeftDirection, invalidMoveTransition };
	hitWallState.Transitions = { onUpDirection, onDownDirection, onLeftDirection, onRightDirection };

	// Set state machine to states it can be in
	stateMachine.States = { upState, downState, leftState, rightState, hitWallState };

	// Set the initial state to down
	stateMachine.InitialState = &downState;
}

void Enemy::Initialize()
{
	LoadSettings();
	SubscribeToEvent(gamelib::PlayerMovedEventTypeEventId);
	SubscribeToEvent(SettingsReloadedEventId);

	// save some frames: don't move every frame. E.g., move every 10ms
	moveTimer.SetFrequency(moveRateMs);

	// Use or create a move strategy
	gameObjectMoveStrategy = gameObjectMoveStrategy == nullptr
		? std::make_shared<GameObjectMoveStrategy>(shared_from_this(), CurrentRoom)
		: gameObjectMoveStrategy;
}

void Enemy::LoadSettings()
{
	emitMoveEvents = gamelib::SettingsManager::Bool("enemy", "emitMoveEvents");
	moveAtSpeed = gamelib::SettingsManager::Bool("enemy", "moveAtSpeed");
	speed = gamelib::SettingsManager::Int("enemy", "speed");
	moveRateMs = gamelib::SettingsManager::Int("enemy", "moveRateMs");
	animate = gamelib::SettingsManager::Bool("enemy", "animate");
	drawState = gamelib::SettingsManager::Bool("enemy", "drawState");
}

std::vector<std::shared_ptr<gamelib::Event>> Enemy::HandleEvent(const std::shared_ptr<gamelib::Event>& event,
	const unsigned long deltaMs)
{
	// Only if the player moves...
	if (event->Id == gamelib::PlayerMovedEventTypeEventId)
	{
		CheckForPlayerCollision();
	}

	// Only if settings event occurs
	if (event->Id.PrimaryId == SettingsReloadedEventId.PrimaryId)
	{
		LoadSettings();
	}

	return {};
}

void Enemy::Update(const unsigned long deltaMs)
{
	if (GameData::Get()->IsGameWon()) return;

	// we only want to move and emit move events periodically
	moveTimer.Update(deltaMs);

	if (moveTimer.IsReady())
	{
		// Automatically keep moving our position in configured direction
		Move(deltaMs);

		moveTimer.Reset();
	}

	// Do common/normal NPC activities also
	Npc::Update(deltaMs);

	// Plus some special Enemy stuff
	auto constexpr emptyString = "";
	auto stateText = drawState
		? stateMachine.ActiveState->GetName().substr(0, 1)
		: emptyString;

	// Set the enemy state text
	Status->Text = stateMachine.ActiveState != nullptr
		? stateText
		: emptyString;
}

bool Enemy::Move(const unsigned long deltaMs)
{
	const std::shared_ptr<gamelib::IMovement> movementAtSpeed = std::make_shared<gamelib::MovementAtSpeed>(speed, currentFacingDirection, deltaMs);
	const std::shared_ptr<gamelib::IMovement> constantPixelMovement = std::make_shared<gamelib::Movement>(currentFacingDirection);

	// Move the game object
	isValidMove = gameObjectMoveStrategy->MoveGameObject(moveAtSpeed
		? movementAtSpeed
		: constantPixelMovement);

	if (isValidMove)
	{
		// Tell the world I moved

		if (!emitMoveEvents) { return true; }

		RaiseEvent(std::make_shared<EnemyMovedEvent>(shared_from_this()));

		return true;
	}

	return false;
}

std::function<bool()> Enemy::IfMovedInDirection(const gamelib::Direction direction) const
{
	// note: returns a func
	return [this, direction] { return IfMoved(direction); };
}

std::function<void(unsigned long deltaMs)> Enemy::DoLookForPlayer()
{
	// note: returns a func
	return [this](const unsigned long) { LookForPlayer(); };
}

bool Enemy::IfMoved(const gamelib::Direction direction) const
{
	return isValidMove && currentFacingDirection == direction;
}

bool Enemy::IsPlayerInSameAxis(const std::shared_ptr<Player>& player, const bool verticalView) const
{
	const auto playerHotspotPosition = player->Hotspot->GetPosition();
	const auto playerHotspotParentHeight = static_cast<int>(player->Hotspot->ParentHeight);
	const auto playerHotspotParentWidth = static_cast<int>(player->Hotspot->ParentWidth);
	constexpr auto half = 2;

	const auto isWithinVerticalFov =
		Hotspot->GetPosition().GetY() > playerHotspotPosition.GetY() + playerHotspotParentHeight / half ||
		Hotspot->GetPosition().GetY() < playerHotspotPosition.GetY() - playerHotspotParentHeight / half;

	const auto isWithinHorizontalFov = 
		Hotspot->GetPosition().GetX() > playerHotspotPosition.GetX() + playerHotspotParentWidth / half ||
		Hotspot->GetPosition().GetX() < playerHotspotPosition.GetX() - playerHotspotParentWidth / half;

	return verticalView ? isWithinVerticalFov : isWithinHorizontalFov;
}

void Enemy::LookForPlayer()
{
	const auto player = GameData::Get()->GetPlayer();
	const auto currentRoom = CurrentRoom->GetCurrentRoom();

	const auto playerRow = player->CurrentRoom->GetCurrentRoom()->GetRowNumber(CurrentLevel->NumRows);
	const auto enemyRow = currentRoom->GetRowNumber(CurrentLevel->NumRows);

	const auto playerCol = player->CurrentRoom->GetCurrentRoom()->GetColumnNumber(CurrentLevel->NumCols);
	const auto enemyCol = currentRoom->GetColumnNumber(CurrentLevel->NumCols);

	// Don't look player unless player is in same row or column
	if (playerRow != enemyRow && playerCol != enemyCol) { return; }

	// Don't look for player if you're already in the room, check for collision
	if (currentRoom->GetRoomNumber() == player->CurrentRoom->RoomIndex)
	{
		CheckForPlayerCollision();
		return;
	}

	// look up and down?
	if (playerCol == enemyCol)
	{
		// Don't look for player if not in the same line as player
		if (IsPlayerInSameAxis(player, false)) { return;}

		// Search for player up.
		if (IsPlayerInLineOfSight(gamelib::Direction::Up))
		{
			SetNpcDirection(gamelib::Direction::Up); // chase in direction found
			return;
		}

		// Search for player down.
		if (IsPlayerInLineOfSight(gamelib::Direction::Down))
		{
			SetNpcDirection(gamelib::Direction::Down); //chase in direction found
			return;
		}
	}

	// look left and right?
	if (playerRow == enemyRow) // look left and right, chase in direction found
	{
		// Don't look for player if not in the same line as player
		if (IsPlayerInSameAxis(player, true)) {return;}

		// Search for player Left.
		if (IsPlayerInLineOfSight(gamelib::Direction::Left))
		{
			SetNpcDirection(gamelib::Direction::Left); //chase in direction found
			return;
		}

		// Search for player right.
		if (IsPlayerInLineOfSight(gamelib::Direction::Right))
		{
			SetNpcDirection(gamelib::Direction::Right); //chase in direction found
		}
	}
}

bool Enemy::IsPlayerInLineOfSight(const gamelib::Direction lookDirection) const
{
	const auto player = GameData::Get()->GetPlayer();

	// Start search in the current room			
	auto currentRoom = CurrentRoom->GetCurrentRoom();
	int nextRoomIndex;

	// Look for line-on-sight in rooms in the direction specified
	// Evaluate each room in that direction to see if its traversable until you reach the end of the map (-1)
	while ((nextRoomIndex = currentRoom->GetNeighborIndex(gamelib::SideUtils::GetSideForDirection(lookDirection))) != -1)
	{
		const auto nextRoom = GameData::Get()->GetRoomByIndex(nextRoomIndex);
		if (lookDirection == gamelib::Direction::Right)
		{
			if (currentRoom->HasRightWall() || nextRoom->HasLeftWall()) return false; // No line of sight
		}

		if (lookDirection == gamelib::Direction::Left)
		{
			if (currentRoom->HasLeftWall() || nextRoom->HasRightWall()) return false; // No line of sight
		}

		if (lookDirection == gamelib::Direction::Up)
		{
			if (currentRoom->HasTopWall() || nextRoom->HasBottomWall()) return false; // No line of sight
		}

		if (lookDirection == gamelib::Direction::Down)
		{
			if (currentRoom->HasBottomWall() || nextRoom->HasTopWall()) return false; // No line of sight
		}

		// pursue next room
		currentRoom = nextRoom;

		// Found the player in this room?
		if (currentRoom->GetRoomNumber() == player->CurrentRoom->RoomIndex)
		{
			// spotted player!
			return true; // Yes!
		}
	}
	return false; // player not found
}

void Enemy::CheckForPlayerCollision()
{
	const auto player = GameDataManager::Get()->GameData()->GetPlayer();

	if (CurrentRoom->RoomIndex == player->CurrentRoom->RoomIndex &&
		SdlCollisionDetection::IsColliding(&player->Bounds, &Bounds))
	{
		RaiseEvent(std::make_shared<PlayerCollidedWithEnemyEvent>(shared_from_this(), player));
		RaiseEvent(GameObjectEventFactory::MakeRemoveObjectEvent(shared_from_this()));		
	}
}

