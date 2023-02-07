#include "pch.h"
#include "Npc.h"

#include <memory>
#include <memory>
#include <memory>
#include <memory>
#include <memory>

#include "RoomInfo.h"
#include "Movement/Movement.h"
#include "objects/AnimatedSprite.h"
#include <events/ControllerMoveEvent.h>

Npc::Npc(const std::string& name, const std::string& type, const gamelib::Coordinate<int> position,
         const bool visible, const std::shared_ptr<Room>& startRoom,
         std::shared_ptr<gamelib::AnimatedSprite> sprite, std::shared_ptr<IGameObjectMoveStrategy> moveStrategy):
DrawableGameObject(name, type, position, visible), Sprite(std::move(sprite)), moveStrategy(std::move(moveStrategy))
{
	SubscribeToEvent(gamelib::EventType::Fire);
	SubscribeToEvent(gamelib::EventType::ControllerMoveEvent);

	CurrentRoom = std::make_shared<RoomInfo>(startRoom);
	SetNpcDirection(gamelib::Direction::Down);

	upState = gamelib::FSMState( []{ gamelib::Logger::Get()->LogThis("NPC Entered Up State");}, []{}, []{gamelib::Logger::Get()->LogThis("NPC Exited Up State");}, "Moving Up" );
	downState = gamelib::FSMState( []{ gamelib::Logger::Get()->LogThis("NPC Entered Down State");}, []{}, []{gamelib::Logger::Get()->LogThis("NPC Exited Down State");}, "Moving Down" );
	leftState = gamelib::FSMState( []{ gamelib::Logger::Get()->LogThis("NPC Entered Left state");}, []{}, []{gamelib::Logger::Get()->LogThis("NPC Exited Left State");}, "Moving Left" );
	rightState = gamelib::FSMState( []{ gamelib::Logger::Get()->LogThis("NPC Entered Right state");}, []{}, []{gamelib::Logger::Get()->LogThis("NPC Exited Right State");}, "Moving Right" );

	onUpDirection = gamelib::FSMTransition( [&]()-> bool { return currentFacingDirection == gamelib::Direction::Up; },
		[&]()-> gamelib::FSMState*{ return &upState; },
		[]{ gamelib::Logger::Get()->LogThis("NPC Transitioning to Up state");  });

	onDownDirection = gamelib::FSMTransition( [&]()-> bool { return currentFacingDirection == gamelib::Direction::Down; },
		[&]()-> gamelib::FSMState*{ return &downState; },
		[]{ gamelib::Logger::Get()->LogThis("NPC Transitioning to Down state");});

	onLeftDirection = gamelib::FSMTransition( [&]()-> bool { return currentFacingDirection == gamelib::Direction::Left; },
		[&]()-> gamelib::FSMState*{ return &leftState; },
		[]{ gamelib::Logger::Get()->LogThis("NPC Transitioning to Left state");});

	onRightDirection = gamelib::FSMTransition( [&]()-> bool { return currentFacingDirection == gamelib::Direction::Right; },
		[&]()-> gamelib::FSMState*{ return &rightState; },
		[]{ gamelib::Logger::Get()->LogThis("NPC Transitioning to Right state");});

	upState.Transitions = { onDownDirection, onLeftDirection, onRightDirection };
	downState.Transitions = { onUpDirection, onLeftDirection, onRightDirection };
	leftState.Transitions = { onUpDirection, onDownDirection, onRightDirection };
	rightState.Transitions = { onUpDirection, onDownDirection, onLeftDirection };

	stateMachine.States = { 	upState, downState, leftState, rightState };
	stateMachine.InitialState = &stateMachine.States.front();
}

void Npc::Initialize()
{
	moveStrategy = moveStrategy == nullptr ? std::make_shared<GameObjectMoveStrategy>(shared_from_this(), CurrentRoom) : moveStrategy;
	Dimensions = Sprite->Dimensions;
	Hotspot = std::make_shared<gamelib::Hotspot>(Position, Dimensions.GetWidth(), Dimensions.GetHeight(), Dimensions.GetWidth() / 2);
	
	CalculateBounds(Position, Dimensions.GetWidth(), Dimensions.GetHeight());
}


void Npc::Draw(SDL_Renderer* renderer)
{
	Sprite->Draw(renderer);
}

void Npc::Update(float deltaMs)
{
	// temp
	Sprite->MoveSprite(Position);
	Sprite->Update(deltaMs, gamelib::AnimatedSprite::GetStdDirectionAnimationFrameGroup(currentFacingDirection));

	// temp		
	Hotspot->Update(Position);

	// temp
	UpdateBounds(Dimensions);

	stateMachine.Update();
}

std::vector<std::shared_ptr<gamelib::Event>> Npc::HandleEvent(const std::shared_ptr<gamelib::Event> event, const unsigned long deltaMs)
{
	if(event->Type == gamelib::EventType::Fire)
	{
		// Swap direction
		if(currentFacingDirection == gamelib::Direction::Up) { SetNpcDirection(gamelib::Direction::Down);}
		else if(currentFacingDirection == gamelib::Direction::Left) { SetNpcDirection(gamelib::Direction::Right);}
		else if(currentFacingDirection == gamelib::Direction::Right) { SetNpcDirection(gamelib::Direction::Left);}
		else if(currentFacingDirection == gamelib::Direction::Down) { SetNpcDirection(gamelib::Direction::Up);}		
	}

	if(event->Type == gamelib::EventType::ControllerMoveEvent)
	{
		const auto moveEvent = std::dynamic_pointer_cast<gamelib::ControllerMoveEvent>(event);	
	
		SetNpcDirection(moveEvent->Direction);
	}
	return DrawableGameObject::HandleEvent(event, deltaMs);
}

void Npc::SetNpcDirection(const gamelib::Direction direction) { currentMovingDirection = direction; currentFacingDirection = direction; }
