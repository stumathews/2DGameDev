#include "Player.h"
#include "PlayerComponent.h"
#include "PlayerMovedEvent.h"
#include "constants.h"
#include "AddGameObjectToCurrentSceneEvent.h"

vector<shared_ptr<Event>> Player::process_event(const std::shared_ptr<Event> event)
{	
	// Process GameObject events
	auto createdEvents(GameObject::process_event(event));

	// Process Square events
	for(auto e : Square::process_event(event)) 
		createdEvents.push_back(e);
	
	// Process Player events
	if(event->m_eventType == PositionChangeEventType)
	{
		auto playerComponent = static_pointer_cast<PlayerComponent>(FindComponent(constants::playerComponentName));		
		auto playerMovedEvent = shared_ptr<PlayerMovedEvent>(new PlayerMovedEvent(playerComponent));

		
		playerComponent->x = get_x();
		playerComponent->y = get_y();
		playerComponent->w = get_w();
		playerComponent->h = get_h();
		player_bounds_.x = playerComponent->x;
		player_bounds_.y = playerComponent->y;
		player_bounds_.w = playerComponent->w;
		player_bounds_.h = playerComponent->h;

		createdEvents.push_back(playerMovedEvent);	
	}
	return createdEvents;
}

void Player::VDraw(SDL_Renderer* renderer)
{
  // Let the player draw itself
	SDL_RenderDrawRect(renderer, &player_bounds_);
}

void Player::add_player_to_game()
{
	/* Schedule adding the player to the screen */
	auto player_width = GlobalConfig::squareWidth / 2;
	auto player_pos_x = 0;
	auto player_pos_y = 0;
	auto player_component = std::make_shared<PlayerComponent>(constants::playerComponentName, player_pos_x, player_pos_y, player_width, player_width);
	auto player_object = std::static_pointer_cast<GameObject>(std::make_shared<Player>(player_component->x, player_component->y, player_component->w));

	player_object->SetTag(constants::playerTag);
	player_object->AddComponent(shared_ptr<Component>(player_component));
	player_object->SubScribeToEvent(PositionChangeEventType);

	/* Add player to scene */
	const auto add_to_scene_event = std::make_shared<AddGameObjectToCurrentSceneEvent>(player_object);
	add_to_scene_event->eventId = 100;
	player_object->raise_event(add_to_scene_event);
}
