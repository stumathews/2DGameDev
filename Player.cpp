#include "Player.h"
#include "player_component.h"
#include "player_moved_event.h"
#include "constants.h"
#include "AddGameObjectToCurrentSceneEvent.h"
#include <memory>
using namespace std;


player::player(int x, int y, int w): square(x, y, w, true, true)
{
	const auto player = std::make_shared<player_component>(constants::playerComponentName, x, y, w, w);
	set_tag(constants::playerTag);
	add_component(player);

	
}

vector<shared_ptr<event>> player::process_event(const std::shared_ptr<event> the_event)
{	
	// Process GameObject events
	auto createdEvents(game_object::process_event(the_event));

	// Process Square events
	for(auto &e : square::process_event(the_event)) 
		createdEvents.push_back(e);
	
	// Process Player events
	if(event_type::PositionChangeEventType == the_event->type)
	{
		auto player = static_pointer_cast<player_component>(find_component(constants::playerComponentName));
		
		player->x = get_x();
		player->y = get_y();
		player->w = get_w();
		player->h = get_h();
		player_bounds_.x = player->x;
		player_bounds_.y = player->y;
		player_bounds_.w = player->w;
		player_bounds_.h = player->h;

		createdEvents.push_back(make_shared<player_moved_event>(player));	
	}
	return createdEvents;
}

void player::draw(SDL_Renderer* renderer)
{
  // Let the player draw itself
	SDL_RenderDrawRect(renderer, &player_bounds_);
}

