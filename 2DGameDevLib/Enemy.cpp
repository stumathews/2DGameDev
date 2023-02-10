#include "pch.h"
#include "Enemy.h"

#include <utility>

#include "GameObjectMoveStrategy.h"
#include "events/ControllerMoveEvent.h"
#include "Movement/Movement.h"

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
			gamelib::Logger::Get()->LogThis(GetName() +" Entered Invalid State");
		},
		[](unsigned long deltaMs){}, [&]
		{
			gamelib::Logger::Get()->LogThis("NPC Exited Invalid State");
			
		},
		"Invalid State" );

	upState = gamelib::FSMState( [&]{ gamelib::Logger::Get()->LogThis(GetName() + " Entered Up State");},
		[&](const unsigned long deltaMs)
		{

		}, []{gamelib::Logger::Get()->LogThis("NPC Exited Up State");},
		"Moving Up" );
	downState = gamelib::FSMState( [&]{ gamelib::Logger::Get()->LogThis(GetName() +" Entered Down State");},
		[&](const unsigned long deltaMs)
		{

		},
		[]{gamelib::Logger::Get()->LogThis("NPC Exited Down State");}, "Moving Down" );
	leftState = gamelib::FSMState( [&]{ gamelib::Logger::Get()->LogThis(GetName() +" Entered Left state");},
		[&](const unsigned long deltaMs)
		{
			
		}, 
		[]{gamelib::Logger::Get()->LogThis("NPC Exited Left State");}, "Moving Left" );
	rightState = gamelib::FSMState( [&]{ gamelib::Logger::Get()->LogThis(GetName() +" Entered Right state");},
		[&](const unsigned long deltaMs)
		{
			
		}, 
		[]{gamelib::Logger::Get()->LogThis("NPC Exited Right State");}, "Moving Right" );

	invalidMoveTransition = gamelib::FSMTransition( [&]()-> bool { return !isValidMove; },
		[&]()-> gamelib::FSMState*{ return &invalidState; },
		[]{ gamelib::Logger::Get()->LogThis("NPC Transitioning to Invalid state");  });

	onUpDirection = gamelib::FSMTransition( [&]()-> bool { return isValidMove && currentFacingDirection == gamelib::Direction::Up; },
		[&]()-> gamelib::FSMState*{ return &upState; },
		[]{ gamelib::Logger::Get()->LogThis("NPC Transitioning to Up state");  });

	onDownDirection = gamelib::FSMTransition( [&]()-> bool { return isValidMove && currentFacingDirection == gamelib::Direction::Down; },
		[&]()-> gamelib::FSMState*{ return &downState; },
		[]{ gamelib::Logger::Get()->LogThis("NPC Transitioning to Down state");});

	onLeftDirection = gamelib::FSMTransition( [&]()-> bool { return isValidMove && currentFacingDirection == gamelib::Direction::Left; },
		[&]()-> gamelib::FSMState*{ return &leftState; },
		[]{ gamelib::Logger::Get()->LogThis("NPC Transitioning to Left state");});

	onRightDirection = gamelib::FSMTransition( [&]()-> bool { return isValidMove && currentFacingDirection == gamelib::Direction::Right; },
		[&]()-> gamelib::FSMState*{ return &rightState; },
		[]{ gamelib::Logger::Get()->LogThis("NPC Transitioning to Right state");});

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
	SubscribeToEvent(gamelib::EventType::Fire);
	
	moveStrategy = moveStrategy == nullptr
		? std::make_shared<GameObjectMoveStrategy>(shared_from_this(), CurrentRoom)
		: moveStrategy;
}


std::vector<std::shared_ptr<gamelib::Event>> Enemy::HandleEvent(const std::shared_ptr<gamelib::Event> event, unsigned long deltaMs)
{
	if(event->Type == gamelib::EventType::Fire)
	{
		SwapCurrentDirection();		
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
