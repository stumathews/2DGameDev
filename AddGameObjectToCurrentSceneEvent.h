#pragma once
#include "Event.h"
#include <memory>
#include "game_object.h"
using namespace std;

class add_game_object_to_current_scene_event : public event
{
public:
	add_game_object_to_current_scene_event(std::shared_ptr<game_object> game_object, int event_id = 0) : event(event_type::AddGameObjectToCurrentScene, event_id)
	{
		set_game_object(game_object);
	}

	void set_game_object(std::shared_ptr<game_object> game_object)
	{
		this->game_object = game_object;
	}
	
	shared_ptr<game_object> get_game_object()
	{
		return game_object;
	}
	string to_str() override;

private:
	shared_ptr<game_object> game_object;
};

inline string add_game_object_to_current_scene_event::to_str()
{
	return "add_game_object_to_current_scene_event";
}
