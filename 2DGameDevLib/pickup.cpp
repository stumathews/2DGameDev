#include "pch.h"
#include "Pickup.h"
#include "events/PlayerMovedEvent.h"
#include <events/GameObjectEvent.h>
#include <common/Common.h>
//#include <objects/DrawableGameObject.h>
#include <scene/SceneManager.h>
#include "player.h"
#include <memory>
#include <SpriteAsset.h>

using namespace std;
namespace gamelib 
{
	Pickup::Pickup(const int x, const int y, const int width, const int height, const bool isVisible) 
			: DrawableGameObject(x, y, isVisible), width(width), height(height)
	{
		//Initialize();
		//auto assetName = components.FindComponent("assetName");

		
	}

	Pickup::Pickup(const bool visible)
		: DrawableGameObject(0, 0, visible), width(0), height(0), fillColour({})
	{
	}

	void Pickup::Initialize()
	{
		// Set the pickups will color on initialization
		fillColour = 
		{
			// Red
			static_cast<Uint8>(SettingsManager::Get()->GetInt("pickup", "r")),
			// Green
			static_cast<Uint8>(SettingsManager::Get()->GetInt("pickup", "g")),
			// Blue
			static_cast<Uint8>(SettingsManager::Get()->GetInt("pickup", "b")),
			// Alpha
			static_cast<Uint8>(SettingsManager::Get()->GetInt("pickup", "a"))
		};
		auto spriteAsset = dynamic_pointer_cast<SpriteAsset>(ResourceManager::Get()->GetAssetInfo(stringProperties["assetName"]));
		sprite = AnimatedSprite::Create(x, y, spriteAsset);;
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

	vector<shared_ptr<gamelib::Event>> Pickup::HandleEvent(shared_ptr<gamelib::Event> event)
	{
		vector<shared_ptr<gamelib::Event>> generated_events;

		// Pickups are concerned with the player moving for collision detection purposes
		if(event->type == gamelib::EventType::PlayerMovedEventType)
		{
			const auto moved_event = std::static_pointer_cast<gamelib::PlayerMovedEvent>(event);
			const auto player = dynamic_pointer_cast<Player>(SceneManager::Get()->GetGameWorld().player);
			SDL_Rect playerHostpotBounds = {player->GetHotspot().GetX(), player->GetHotspot().GetY(), 1, 1};
			SDL_Rect result;
			
			// Basic collision detection
			if(SDL_IntersectRect(&playerHostpotBounds, &bounds, &result))
			{
				generated_events.push_back(make_shared<gamelib::Event>(gamelib::EventType::FetchedPickup));
				generated_events.push_back(make_shared<gamelib::GameObjectEvent>(id, this, gamelib::GameObjectEventContext::Remove));
			}
		
		}

		if(event->type == gamelib::EventType::DoLogicUpdateEventType)
		{
			// Update bounds etc.
			Update(0.0f);
		}
		return generated_events;
	}

	void Pickup::LoadSettings() 	{ }

	void Pickup::Draw(SDL_Renderer* renderer)
	{
		// Collect our dimensions before drawing with them
		SDL_Rect dimensions = 
		{
			x,
			y,
			width,
			height
		};
	
		// Draw 
		//DrawFilledRect(renderer, &dimensions, fillColour);
		sprite->Draw(renderer);
	}

	void Pickup::Update(float deltaMs)
	{
		bounds = 
		{ 
			x, 
			y,
			width,
			height
		};

		sprite->x = x;
		sprite->y = y;
		sprite->Update(deltaMs);
	}
}