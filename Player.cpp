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
	auto created_events(game_object::process_event(the_event));

	// Process Square events
	for(auto &e : square::process_event(the_event)) 
		created_events.push_back(e);
	
	// Process Player events
	if(event_type::PositionChangeEventType == the_event->type)
	{
		auto player = static_pointer_cast<player_component>(find_component(constants::playerComponentName));
		
		player_bounds_.x = player->x = get_x();
		player_bounds_.y = player->y = get_y();
		player_bounds_.w = player->w = get_w();
		player_bounds_.h = player->h = get_h();

		created_events.push_back(make_shared<player_moved_event>(player));	
	}
	return created_events;
}

void player::draw(SDL_Renderer* renderer)
{
  // Let the player draw itself
	SDL_RenderDrawRect(renderer, &player_bounds_);
}

string player::get_identifier()
{
	return "square";
}

