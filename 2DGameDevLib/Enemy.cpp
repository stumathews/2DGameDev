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
             std::shared_ptr<gamelib::IGameObjectMoveStrategy> enemyMoveStrategy): Npc(name, type, position, visible,
                                                                                       std::move(sprite),
                                                                                       std::move(enemyMoveStrategy))
{
	SetNpcDirection(startingDirection);
	CurrentRoom = std::make_shared<RoomInfo>(startRoom);

	invalidState = gamelib::FSMState( [&]
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
		[&]()-> gamelib::FSMState*{ return &invalidState; },
		[]{   });

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

	upState.Transitions = { onDownDirection, onLeftDirection, onRightDirection, invalidMoveTransition };
	downState.Transitions = { onUpDirection, onLeftDirection, onRightDirection, invalidMoveTransition };
	leftState.Transitions = { onUpDirection, onDownDirection, onRightDirection, invalidMoveTransition };
	rightState.Transitions = { onUpDirection, onDownDirection, onLeftDirection, invalidMoveTransition };
	invalidState.Transitions = {onUpDirection, onDownDirection, onLeftDirection, onRightDirection};

	stateMachine.States = { 	upState, downState, leftState, rightState, invalidState };
	stateMachine.InitialState = &stateMachine.States.front();
}

void Enemy::LookForPlayer()
{
	const auto player = GameData::Get()->GetPlayer();			
			auto currentRoom = CurrentRoom->GetCurrentRoom();

			const auto playerRow = player->CurrentRoom->GetCurrentRoom()->GetRowNumber(10);
			const auto enemyRow = currentRoom->GetRowNumber(10);

			const auto playerCol = player->CurrentRoom->GetCurrentRoom()->GetColumnNumber(10);
			const auto enemyCol = currentRoom->GetColumnNumber(10);

			const auto oppositeDirection = gamelib::DirectionUtils::GetOppositeDirectionTo(currentFacingDirection);
			const auto oppositeSide = gamelib::SideUtils::GetOppositeSideForDirection(currentFacingDirection);

			if(currentFacingDirection == gamelib::Direction::Left || currentFacingDirection == gamelib::Direction::Right)
			{
				if(playerRow != enemyRow)
				{
					return;
				}
			}

			if(currentFacingDirection == gamelib::Direction::Up || currentFacingDirection == gamelib::Direction::Down)
			{
				if(playerCol != enemyCol)
				{
					return;
				}
			}

			if(currentMovingDirection == oppositeDirection)
			{
				return;
			}

	        // Don't look for player if you're already in the room, check for collision
			if(currentRoom->GetRoomNumber() == player->CurrentRoom->RoomIndex)
			{
				CheckForPlayerCollision();
				return;
			}

			int nextRoomIndex;
			while((nextRoomIndex = currentRoom->GetNeighbourIndex(oppositeSide)) != -1)
			{
				if(currentRoom->GetRoomNumber() == player->CurrentRoom->RoomIndex)
				{
					gamelib::Logger::Get()->LogThis("Spotted player!");
					SwapCurrentDirection();
				}
				const auto nextRoom =  GameData::Get()->GetRoomByIndex(nextRoomIndex);
				if(currentFacingDirection == gamelib::Direction::Right)
				{
					if(currentRoom->HasLeftWall() || nextRoom->HasRightWall()) return;
				}
				if(currentFacingDirection == gamelib::Direction::Left)
				{
					if(currentRoom->HasRightWall() || nextRoom->HasLeftWall()) return;
				}
				if(currentFacingDirection == gamelib::Direction::Up)
				{
					if(currentRoom->HasBottomWall() || nextRoom->HasTopWall()) return;
				}
				if(currentFacingDirection == gamelib::Direction::Down)
				{
					if(currentRoom->HasTopWall() || nextRoom->HasBottomWall()) return;
				}
				// pursue next room
				currentRoom = nextRoom;
			}
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
