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
#include <events/DoLogicUpdateEvent.h>

using namespace std;

namespace gamelib 
{
	Pickup::Pickup(const int x, const int y, const int width, const int height, const bool isVisible) : DrawableGameObject(x, y, isVisible)
	{
		this->width = width;
		this->height = height;
		fillColour = { };		
	}

	Pickup::Pickup(const bool visible) : DrawableGameObject(0, 0, visible)
	{
		this->width = width;
		this->height = height;
		fillColour = { };	
	}

	void Pickup::Initialize()
	{
		fillColour = 
		{			
			static_cast<Uint8>(SettingsManager::Get()->GetInt("pickup", "r")),			
			static_cast<Uint8>(SettingsManager::Get()->GetInt("pickup", "g")),			
			static_cast<Uint8>(SettingsManager::Get()->GetInt("pickup", "b")),			
			static_cast<Uint8>(SettingsManager::Get()->GetInt("pickup", "a"))
		};

		auto spriteAsset = dynamic_pointer_cast<SpriteAsset>(ResourceManager::Get()->GetAssetInfo(stringProperties["assetName"]));
		sprite = AnimatedSprite::Create(Position.GetX(), Position.GetY(), spriteAsset);;
	}

	gamelib::ListOfEvents Pickup::HandleEvent(shared_ptr<gamelib::Event> event)
	{
		gamelib::ListOfEvents generated_events;

		const auto moved_event = std::static_pointer_cast<gamelib::PlayerMovedEvent>(event);
		const auto player = dynamic_pointer_cast<Player>(SceneManager::Get()->GetGameWorld().player);

		// TODO: We should have a dedicated hostspot manager
		SDL_Rect playerHostpotBounds = { player->GetHotspot().GetX(), player->GetHotspot().GetY(), 1, 1};

		switch(event->type)
		{
			case gamelib::EventType::PlayerMovedEventType:				
				SDL_Rect result;
			
				// Basic collision detection (check if the player moved into me)				
				if (player->GetCurrentRoom()->GetRoomNumber() == RoomNumber)
				{
					if (SDL_IntersectRect(&playerHostpotBounds, &Bounds, &result))
					{
						generated_events.push_back(make_shared<gamelib::Event>(gamelib::EventType::FetchedPickup));
						generated_events.push_back(make_shared<gamelib::GameObjectEvent>(Id, this, gamelib::GameObjectEventContext::Remove));
					}
				}
				break;
			case gamelib::EventType::DoLogicUpdateEventType:

				auto updateInfo = std::static_pointer_cast<gamelib::LogicUpdateEvent>(event);
				Update(updateInfo->deltaMs);
				break;

		}
		return generated_events;
	}

	void Pickup::Draw(SDL_Renderer* renderer)
	{
		if(!sprite)
		{
			SDL_Rect dimensions = { Position.GetX(), Position.GetY(), width, height };		 
			DrawFilledRect(renderer, &dimensions, fillColour);
			return;
		}
		
		sprite->Draw(renderer);		
	}

	void Pickup::Update(float deltaMs)
	{
		Bounds = { Position.GetX(), Position.GetY(), width, height };

		sprite->Position.SetX(Position.GetX());
		sprite->Position.SetY(Position.GetY());

		sprite->Update(deltaMs);
	}

	std::string Pickup::GetSubscriberName() 
	{ 
		return "pickup";
	}

	gamelib::GameObjectType Pickup::GetGameObjectType() 
	{ 
		return gamelib::GameObjectType::Pickup; 
	}

	std::string Pickup::GetName()
	{
		return "pickup";
	}	

	void Pickup::LoadSettings() {}
}