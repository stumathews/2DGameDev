#include "pch.h"
#include "Enemy.h"

#include <memory>
#include <utility>

#include "GameDataManager.h"
#include "GameObjectMoveStrategy.h"
#include "Player.h"
#include "events/ControllerMoveEvent.h"
#include "events/GameObjectEvent.h"
#include "Movement/Movement.h"
#include <events/Events.h>

#include "EventNumber.h"

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

		}, []{},
		"Moving Up" );
	downState = gamelib::FSMState( [&]{ },
		[&](const unsigned long deltaMs)
		{

		},
		[]{}, "Moving Down" );
	leftState = gamelib::FSMState( [&]{ },
		[&](const unsigned long deltaMs)
		{
			
		}, 
		[]{}, "Moving Left" );
	rightState = gamelib::FSMState( [&]{ },
		[&](const unsigned long deltaMs)
		{
			
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

void Enemy::Initialize()
{
	SubscribeToEvent(FireEventId);
	SubscribeToEvent(gamelib::PlayerMovedEventTypeEventId);
	
	moveStrategy = moveStrategy == nullptr
		? std::make_shared<GameObjectMoveStrategy>(shared_from_this(), CurrentRoom)
		: moveStrategy;
}


std::vector<std::shared_ptr<gamelib::Event>> Enemy::HandleEvent(const std::shared_ptr<gamelib::Event> event, unsigned long deltaMs)
{
	if(event->Id.Id == FireEventId.Id)
	{
		SwapCurrentDirection();		
	}

	if(event->Id.Id == gamelib::PlayerMovedEventTypeEventId.Id)
	{
		const auto player = GameDataManager::Get()->GameData()->GetPlayer();
		SDL_Rect result;
		if(CurrentRoom->RoomIndex == player->CurrentRoom->RoomIndex && SDL_IntersectRect(&player->Bounds, &Bounds, &result))
		{
			RaiseEvent(std::make_shared<gamelib::GameObjectEvent>(shared_from_this(), gamelib::GameObjectEventContext::Remove));
		}
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
