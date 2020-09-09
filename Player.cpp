#include "Player.h"
#include "PlayerComponent.h"
#include "PlayerMovedEvent.h"
#include "constants.h"
#include "AddGameObjectToCurrentSceneEvent.h"

Player::Player(int x, int y, int w): square(x, y, w, true, true)
{
	const auto player_component = std::make_shared<PlayerComponent>(constants::playerComponentName, x, y, w, w);
	set_tag(constants::playerTag);
	add_component(player_component);
	subscribe_to_event(PositionChangeEventType);

	add_player_to_scene();
	
}

vector<shared_ptr<Event>> Player::process_event(const std::shared_ptr<Event> event)
{	
	// Process GameObject events
	auto createdEvents(game_object::process_event(event));

	// Process Square events
	for(auto e : square::process_event(event)) 
		createdEvents.push_back(e);
	
	// Process Player events
	if(PositionChangeEventType == event->m_eventType)
	{
		auto player_component = static_pointer_cast<PlayerComponent>(find_component(constants::playerComponentName));
		const auto player_moved_event = std::make_shared<PlayerMovedEvent>(player_component);

		
		player_component->x = get_x();
		player_component->y = get_y();
		player_component->w = get_w();
		player_component->h = get_h();
		player_bounds_.x = player_component->x;
		player_bounds_.y = player_component->y;
		player_bounds_.w = player_component->w;
		player_bounds_.h = player_component->h;

		createdEvents.push_back(player_moved_event);	
	}
	return createdEvents;
}

void Player::draw(SDL_Renderer* renderer)
{
  // Let the player draw itself
	SDL_RenderDrawRect(renderer, &player_bounds_);
}

void Player::add_player_to_scene() const
{
	/* Schedule adding the player to the screen */
	auto player_width = global_config::square_width / 2;
	auto player_pos_x = 100;
	auto player_pos_y = 100;
	auto player_object = std::static_pointer_cast<game_object>(std::make_shared<Player>(player_pos_x, player_pos_y, player_width));

	/* Add player to scene */
	const auto add_to_scene_event = std::make_shared<AddGameObjectToCurrentSceneEvent>(player_object);
	add_to_scene_event->eventId = 100;
	player_object->raise_event(add_to_scene_event);
}
