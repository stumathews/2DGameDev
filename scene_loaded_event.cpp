#include "scene_loaded_event.h"

using namespace std;

scene_loaded_event::scene_loaded_event(int scene_id): event(event_type::scene_loaded), scene_id_(scene_id)
{
}

string scene_loaded_event::to_str()
{
	return "scene_loaded_event";
}
