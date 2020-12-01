#include <SDL.h>
#include <Windows.h>
#include <iostream>
#include "Common.h"
#include "game_object.h"
#include <vector>
#include "game_structure.h"
#include "scene_manager.h"
#include "resource_manager.h"
#include "AudioManager.h"
#include "font_manager.h"

using namespace std;

shared_ptr<game_world_data> game_world(new game_world_data); // shared game world
shared_ptr<event_manager> event_admin(new event_manager); // co-ordinates all events within the game
shared_ptr<scene_manager> scene_admin(new scene_manager); // co-ordinates the scene-related activities in the game
shared_ptr<resource_manager> resource_admin(new resource_manager); // co-ordinates the resources in the game - such as holding definitions of all the resources/assets in the game
shared_ptr<game_structure> game(new game_structure); // logical structure of the game such as the game initialization, game loop etc.
shared_ptr<sdl_graphics_manager> graphics_admin(new sdl_graphics_manager); // deals with our drawing operations
shared_ptr<audio_manager> audio_admin(new audio_manager); // makes audio resources
shared_ptr<font_manager> font_admin(new font_manager); // makes font resources
shared_ptr<global_config> config(new global_config); // game settings

int main(int argc, char *args[])
{
	const auto is_initialized = succeeded(game->initialize(), "Initialize");
	const auto is_loaded = succeeded(game->load_content(), "load_content");

	if(!is_initialized || !is_loaded)
		return -1;

	run_and_log("Beginning game loop...", config->verbose, [&]()
	{
		game->game_loop();
		return true;
	});

	run_and_log("Unloading game...", config->verbose, [&]()
	{
		game->unload();	
		return true;
	});
	
	return 0;
}