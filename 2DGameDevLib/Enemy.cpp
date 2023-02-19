#include "pch.h"
#include "Enemy.h"

#include <memory>
#include <utility>

#include "DirectionUtils.h"
#include "GameDataManager.h"
#include "GameObjectMoveStrategy.h"
#include "Player.h"
#include "events/ControllerMoveEvent.h"
#include "events/GameObjectEvent.h"
#include "Movement/Movement.h"

#include "PlayerCollidedWithEnemyEvent.h"
#include "EventNumber.h"
#include "geometry/SideUtils.h"
#include "Room.h"

namespace gamelib
{
	class ControllerMoveEvent;
}

Enemy::Enemy(const std::string& name, const std::string& type, const gamelib::Coordinate<int> position, const bool visible,
             const std::shared_ptr<Room>& startRoom, std::shared_ptr<gamelib::AnimatedSprite> sprite, const gamelib::Direction startingDirection,
		    std::shared_ptr<const Level> level,         
			std::shared_ptr<gamelib::IGameObjectMoveStrategy> enemyMoveStrategy): Npc(
	name, type, position, visible, std::move(sprite), std::move(enemyMoveStrategy)), CurrentLevel(std::move(level))
{
	SetNpcDirection(startingDirection);
	CurrentRoom = std::make_shared<RoomInfo>(startRoom);

	HitWallState = gamelib::FSMState( [&]
		{
			SwapCurrentDirection();
		},
		[](unsigned long deltaMs){}, [&]
		{

		},
		"Invalid State" );

	upState = gamelib::FSMState( [&]{ },
		[&](const unsigned long deltaMs)
		{
			LookForPlayer();
		}, []{},
		"Moving Up" );
	downState = gamelib::FSMState( [&]{ },
		[&](const unsigned long deltaMs)
		{
			LookForPlayer();
		},
		[]{}, "Moving Down" );
	leftState = gamelib::FSMState( [&]{ },
		[&](const unsigned long deltaMs)
		{
			LookForPlayer();
		}, 
		[]{}, "Moving Left" );
	rightState = gamelib::FSMState( [&]{ },
		[&](const unsigned long deltaMs)
		{
			LookForPlayer();

		}, 
		[]{}, "Moving Right" );

	invalidMoveTransition = gamelib::FSMTransition( [&]()-> bool { return !isValidMove; },
		[&]()-> gamelib::FSMState*{ return &HitWallState; },
		[]{   });

	// Set the state depending on which direction the enemy is facing
	onUpDirection = gamelib::FSMTransition( [&]()-> bool { return isValidMove && currentFacingDirection == gamelib::Direction::Up; },
		[&]()-> gamelib::FSMState*{ return &upState; },
		[]{   });

	onDownDirection = gamelib::FSMTransition( [&]()-> bool { return isValidMove && currentFacingDirection == gamelib::Direction::Down; },
		[&]()-> gamelib::FSMState*{ return &downState; },
		[]{ });

	onLeftDirection = gamelib::FSMTransition( [&]()-> bool { return isValidMove && currentFacingDirection == gamelib::Direction::Left; },
		[&]()-> gamelib::FSMState*{ return &leftState; },
		[]{ });

	onRightDirection = gamelib::FSMTransition( [&]()-> bool { return isValidMove && currentFacingDirection == gamelib::Direction::Right; },
		[&]()-> gamelib::FSMState*{ return &rightState; },
		[]{ });

	// Configure valid transitions
	upState.Transitions = { onDownDirection, onLeftDirection, onRightDirection, invalidMoveTransition };
	downState.Transitions = { onUpDirection, onLeftDirection, onRightDirection, invalidMoveTransition };
	leftState.Transitions = { onUpDirection, onDownDirection, onRightDirection, invalidMoveTransition };
	rightState.Transitions = { onUpDirection, onDownDirection, onLeftDirection, invalidMoveTransition };
	HitWallState.Transitions = {onUpDirection, onDownDirection, onLeftDirection, onRightDirection};

	// Set state machine to states it can be in
	stateMachine.States = { 	upState, downState, leftState, rightState, HitWallState };

	// Set the initial state to the first one
	stateMachine.InitialState = &stateMachine.States.front();
}

void Enemy::LookForPlayer()
{
	const auto player = GameData::Get()->GetPlayer();
	const auto currentRoom = CurrentRoom->GetCurrentRoom();

	const auto playerRow = player->CurrentRoom->GetCurrentRoom()->GetRowNumber(CurrentLevel->NumRows);
	const auto enemyRow = currentRoom->GetRowNumber(CurrentLevel->NumRows);

	const auto playerCol = player->CurrentRoom->GetCurrentRoom()->GetColumnNumber(CurrentLevel->NumCols);
	const auto enemyCol = currentRoom->GetColumnNumber(CurrentLevel->NumCols);

	// Don't look player unless player is un same row or column
	if(playerRow != enemyRow && playerCol != enemyCol) { return; }

	// Don't look for player if already spotted in the direction we are moving
	if(PlayerLastSpottedDirection == currentMovingDirection) { return; }

    // Don't look for player if you're already in the room, check for collision
	if(currentRoom->GetRoomNumber() == player->CurrentRoom->RoomIndex)
	{
		CheckForPlayerCollision();
		return;
	}
	
	if(playerCol == enemyCol) // look up and down, chase in direction found
	{
		// Don't look for player if not in the same line as player
		if(Hotspot->GetPosition().GetX() != player->Hotspot->GetPosition().GetX()) return;

		// Search for player up.
		if(LookForPlayerInDirection(gamelib::Direction::Up))
		{			
			SetNpcDirection(gamelib::Direction::Up);
			PlayerLastSpottedDirection = gamelib::Direction::Up;
			return;
		}

		// Search for player down.
		if(LookForPlayerInDirection(gamelib::Direction::Down)) 
		{
			SetNpcDirection(gamelib::Direction::Down);
			PlayerLastSpottedDirection = gamelib::Direction::Down;
			return;
		}
	}
	else if (playerRow == enemyRow) // look left and right, chase in direction found
	{
		// Don't look for player if not in the same line as player
		if(Hotspot->GetPosition().GetY() != player->Hotspot->GetPosition().GetY()) return;

		// Search for player Left.
		if(LookForPlayerInDirection(gamelib::Direction::Left))
		{
			SetNpcDirection(gamelib::Direction::Left);
			PlayerLastSpottedDirection = gamelib::Direction::Left;
			return;
		}

		// Search for player right.
		if(LookForPlayerInDirection(gamelib::Direction::Right)) 
		{
			SetNpcDirection(gamelib::Direction::Right);
			PlayerLastSpottedDirection = gamelib::Direction::Right;
			return;
		}
	}
}

bool Enemy::LookForPlayerInDirection(const gamelib::Direction lookDirection) const
{
	const auto player = GameData::Get()->GetPlayer();

	// Start search in the current room			
	auto currentRoom = CurrentRoom->GetCurrentRoom();
	int nextRoomIndex;

	// Look for line-on-sight in rooms in the direction specified
	while((nextRoomIndex = currentRoom->GetNeighbourIndex(gamelib::SideUtils::GetSideForDirection(lookDirection))) != -1)
	{		
		const auto nextRoom =  GameData::Get()->GetRoomByIndex(nextRoomIndex);
		if(lookDirection == gamelib::Direction::Right)
		{
			if(currentRoom->HasRightWall() || nextRoom->HasLeftWall()) return false; // No line of sight
		}
		if(lookDirection == gamelib::Direction::Left)
		{			
			if(currentRoom->HasLeftWall() || nextRoom->HasRightWall()) return false; // No line of sight
		}
		if(lookDirection == gamelib::Direction::Up)
		{			
			if(currentRoom->HasTopWall() || nextRoom->HasBottomWall()) return false; // No line of sight
		}
		if(lookDirection == gamelib::Direction::Down)
		{
			if(currentRoom->HasBottomWall() || nextRoom->HasTopWall()) return false; // No line of sight
		}

		// pursue next room
		currentRoom = nextRoom;

		// Found the player in this room?
		if(currentRoom->GetRoomNumber() == player->CurrentRoom->RoomIndex)
		{
			gamelib::Logger::Get()->LogThis("Spotted player!");
			return true; // Yes!
		}
	}
	return false; // player not found
}

void Enemy::Initialize()
{
	SubscribeToEvent(FireEventId);
	SubscribeToEvent(gamelib::PlayerMovedEventTypeEventId);
	
	moveStrategy = moveStrategy == nullptr
		? std::make_shared<GameObjectMoveStrategy>(shared_from_this(), CurrentRoom)
		: moveStrategy;
}


void Enemy::CheckForPlayerCollision()
{
	const auto player = GameDataManager::Get()->GameData()->GetPlayer();
	SDL_Rect result;
	if(CurrentRoom->RoomIndex == player->CurrentRoom->RoomIndex && SDL_IntersectRect(&player->Bounds, &Bounds, &result))
	{
		RaiseEvent(std::make_shared<PlayerCollidedWithEnemyEvent>(shared_from_this(), player));
		RaiseEvent(std::make_shared<gamelib::GameObjectEvent>(shared_from_this(), gamelib::GameObjectEventContext::Remove));			
	}
}

std::vector<std::shared_ptr<gamelib::Event>> Enemy::HandleEvent(const std::shared_ptr<gamelib::Event> event, unsigned long deltaMs)
{
	if(event->Id == FireEventId)
	{
		SwapCurrentDirection();		
	}

	if(event->Id == gamelib::PlayerMovedEventTypeEventId)
	{
		CheckForPlayerCollision();
	}

	return {};
}

void Enemy::Update(const unsigned long deltaMs)
{
	isValidMove = moveStrategy->MoveGameObject(std::make_shared<Movement>(currentFacingDirection));		
	Hotspot->Update(Position);
	Sprite->MoveSprite(Position);
	Sprite->Update(deltaMs, gamelib::AnimatedSprite::GetStdDirectionAnimationFrameGroup(currentFacingDirection));
	UpdateBounds(Dimensions);
	stateMachine.Update(deltaMs);
}
