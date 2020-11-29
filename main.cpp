#include <SDL.h>
#include <Windows.h>
#include <iostream>
#include "Common.h"
#include "game_object.h"
#include <vector>
#include "game_structure.h"
#include "scene_manager.h"
#include "resource_manager.h"

using namespace std;

shared_ptr<event_manager> event_admin(new event_manager);
shared_ptr<scene_manager> scene_admin(new scene_manager);
shared_ptr<resource_manager> resource_admin(new resource_manager);

int main(int argc, char *args[])
{
	const auto is_initialized = succeeded(game_structure::initialize());
	const auto is_loaded = succeeded(game_structure::load_content());

	if(!is_initialized || !is_loaded)
		return -1;
	
	game_structure::game_loop();	
	game_structure::unload();	
	return 0;
}