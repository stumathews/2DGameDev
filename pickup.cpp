#include "Pickup.h"
#include "events/PlayerMovedEvent.h"
#include <events/GameObjectEvent.h>
#include <common/Common.h>
//#include <objects/DrawableGameObject.h>

using namespace std;
namespace gamelib 
{
	Pickup::Pickup(const int x, const int y, const int width, const int height, const bool isVisible) 
			: DrawableGameObject(x, y, isVisible), width(width), height(height)
	{
		Initialize();
	}

	Pickup::Pickup(const bool visible)
		: DrawableGameObject(0, 0, visible), width(0), height(0)
	{
		Initialize();
	}

	void Pickup::Initialize()
	{
		// Set the pickups will color on initialization
		fillColour = 
		{
			// Red
			static_cast<Uint8>(SettingsManager::Get()->get_int("pickup", "r")),
			// Green
			static_cast<Uint8>(SettingsManager::Get()->get_int("pickup", "g")),
			// Blue
			static_cast<Uint8>(SettingsManager::Get()->get_int("pickup", "b")),
			// Alpha
			static_cast<Uint8>(SettingsManager::Get()->get_int("pickup", "a"))
		};
	}

	std::string Pickup::GetSubscriberName() 
	{ 
		return "pickup";
	}

	gamelib::object_type Pickup::GetGameObjectType() 
	{ 
		return gamelib::object_type::Pickup; 
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
			const auto player = moved_event->get_player_component()->the_player;

			// Basic collision detection
			if(player->x == x && player->y == y)
			{
				generated_events.push_back(make_shared<gamelib::Event>(gamelib::EventType::FetchedPickup));
				generated_events.push_back(make_shared<gamelib::GameObjectEvent>(id, this, gamelib::GameObjectEventContext::Remove));
			}
		
		}

		if(event->type == gamelib::EventType::DoLogicUpdateEventType)
		{
			// Update bounds etc.
			Update();
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
		DrawFilledRect(renderer, &dimensions, fillColour);
	}

	void Pickup::Update()
	{
		bounds = 
		{ 
			x, 
			y,
			width,
			height
		};
	}
}