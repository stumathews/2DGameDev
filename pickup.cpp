#include "Pickup.h"
#include "events/player_moved_event.h"
#include <events/GameObjectEvent.h>
#include <common/Common.h>

using namespace std;

Pickup::Pickup(const int x, const int y, const int w, const int h, const bool visible,  std::shared_ptr<gamelib::event_manager> event_admin, const std::shared_ptr<gamelib::settings_manager>& settings_admin)
	: DrawingBase(x, y, visible, settings_admin), width(w), height(h), event_admin(std::move(event_admin))
{
	init();
}

void Pickup::init()
{
	event_admin->subscribe_to_event(gamelib::event_type::DoLogicUpdateEventType, this);
	fill_color = {
			static_cast<Uint8>(settings_admin->get_int("pickup", "r")),
			static_cast<Uint8>(settings_admin->get_int("pickup", "g")),
			static_cast<Uint8>(settings_admin->get_int("pickup", "b")),
			static_cast<Uint8>(settings_admin->get_int("pickup", "a"))			
		};
}

Pickup::Pickup(const bool visible, const shared_ptr<gamelib::settings_manager>& settings_admin):
	DrawingBase(0, 0, visible, settings_admin), width(0), height(0)
{
	init();
}


vector<shared_ptr<gamelib::event>> Pickup::handle_event(shared_ptr<gamelib::event> the_event)
{
	vector<shared_ptr<gamelib::event>> generated_events;

	if(the_event->type == gamelib::event_type::PlayerMovedEventType)
	{
		const auto moved_event = std::static_pointer_cast<gamelib::player_moved_event>(the_event);				
			const auto player_component = moved_event->get_player_component();
			const auto player = player_component->the_player;

		// basic little collision detection
		if(player->x == x && player->y == y)
		{
			generated_events.push_back(make_shared<gamelib::event>(gamelib::event_type::FetchedPickup));
			generated_events.push_back(make_shared<gamelib::GameObjectEvent>(id, this, gamelib::GameObjectEventContext::Remove));
		}
		
	}
	if(the_event->type == gamelib::event_type::DoLogicUpdateEventType)
	{
		update();
	}
	return generated_events;
}

void Pickup::load_settings(shared_ptr<gamelib::settings_manager> settings_admin)
{
	
}

void Pickup::draw(SDL_Renderer* renderer)
{
	SDL_Rect rect = { x, y, width, height};
	
	DrawFilledRect(renderer, &rect, fill_color);
}

void Pickup::update()
{
	bounds = { x, y, width, height};
}

Pickup::~Pickup()
{
	gamelib::log_message("~Pickup()", settings_admin->get_bool("global","verbose"));
	
	event_admin->remove_subscription(id, gamelib::event_type::GameObject);
}
