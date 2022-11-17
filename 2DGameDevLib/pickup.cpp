#include "pickup.h"
#include "pch.h"
#include "Pickup.h"
#include "events/PlayerMovedEvent.h"
#include <events/GameObjectEvent.h>
#include <common/Common.h>
#include <scene/SceneManager.h>
#include "player.h"
#include <memory>
#include <SpriteAsset.h>
#include <common/aliases.h>
#include <events/UpdateAllGameObjectsEvent.h>

using namespace std;

namespace gamelib 
{
	Pickup::Pickup(const int x, const int y, const int width, const int height, const bool isVisible): DrawableGameObject(x, y, isVisible)
	{
		this->width = width;
		this->height = height;
		this->RoomNumber = 0;
		this->fillColour = {0};		
	}

	Pickup::Pickup(const bool visible) : DrawableGameObject(0, 0, visible)
	{
		this->width = 0;
		this->height = 0;
		this->RoomNumber = 0;
		this->fillColour = {0};	
	}

	void Pickup::Initialize()
	{		
		// Create the sprite from the asset associated with pickup
		sprite = AnimatedSprite::Create(Position.GetX(), Position.GetY(), 
			dynamic_pointer_cast<SpriteAsset>(ResourceManager::Get()->GetAssetInfo(stringProperties["assetName"])));		
	}

	gamelib::ListOfEvents Pickup::HandleEvent(shared_ptr<gamelib::Event> event)
	{
		gamelib::ListOfEvents generated_events;

		switch(event->type)
		{
			case gamelib::EventType::PlayerMovedEventType:	
			{
				SDL_Rect result;

				const auto player = dynamic_pointer_cast<Player>(SceneManager::Get()->GetGameWorld().player);

				// Basic collision detection (check if the player moved into me)				
				if (player->GetCurrentRoom()->GetRoomNumber() == RoomNumber)
				{
					if (SDL_IntersectRect(&player->Bounds, &Bounds, &result))
					{
						generated_events.push_back(make_shared<gamelib::Event>(gamelib::EventType::FetchedPickup));
						generated_events.push_back(make_shared<gamelib::GameObjectEvent>(Id, this, gamelib::GameObjectEventContext::Remove));
					}
				}
			}
			break;
		}
		return generated_events;
	}

	void Pickup::Draw(SDL_Renderer* renderer)
	{
		sprite->Draw(renderer);		
	}

	void Pickup::UpdateBounds()
	{
		Bounds = { Position.GetX(), Position.GetY(), width, height };
	}

	void Pickup::Update(float deltaMs)
	{
		UpdateBounds();		

		sprite->Position.SetX(Position.GetX());
		sprite->Position.SetY(Position.GetY());

		// Progress sprite frame time
		sprite->Update(deltaMs);
	}

}