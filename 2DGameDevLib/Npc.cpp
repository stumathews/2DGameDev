#include "pch.h"
#include "Npc.h"

#include <memory>
#include <memory>
#include <memory>

#include "RoomInfo.h"
#include "Movement/Movement.h"
#include "objects/AnimatedSprite.h"

Npc::Npc(const std::string& name, const std::string& type, const gamelib::Coordinate<int> position,
         const bool visible, const int): DrawableGameObject(name, type, position, visible)
{
	SubscribeToEvent(gamelib::EventType::Fire);
	CurrentRoom = std::make_shared<RoomInfo>();
}


void Npc::SetMoveStrategy(const std::shared_ptr<IGameObjectMoveStrategy>& inMoveStrategy) { moveStrategy = inMoveStrategy; }

void Npc::SetSprite(const std::shared_ptr<gamelib::AnimatedSprite>& inSprite)
{
	Sprite = inSprite;
	Width = inSprite->Dimensions.GetWidth();
	Height = inSprite->Dimensions.GetHeight();
	Hotspot = std::make_shared<gamelib::Hotspot>(Position, Width, Height, Width / 2);
	
	CalculateBounds(Position, Width, Height);	
}

void Npc::Draw(SDL_Renderer* renderer)
{
	Sprite->Draw(renderer);
}

void Npc::Update(float deltaMs)
{
	
}

std::vector<std::shared_ptr<gamelib::Event>> Npc::HandleEvent(const std::shared_ptr<gamelib::Event> event, const unsigned long deltaMs)
{
	if(event->Type == gamelib::EventType::Fire)
	{
		// temp
		moveStrategy->MoveGameObject(std::make_shared<Movement>(gamelib::Direction::Up, 10));
		// temp
		Sprite->MoveSprite(Position.GetX(), Position.GetY());
		// temp
		UpdateBounds(Width, Height);
	}
	return DrawableGameObject::HandleEvent(event, deltaMs);
}
