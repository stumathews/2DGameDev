#include <SDL.h>
#include <Windows.h>
#include <iostream>
#include "common/Common.h"
#include "game_structure.h"
#include "font/font_manager.h"
#include <events/event_manager.h>
#include "audio/AudioManager.h"
#include "game/exceptions/game_exception.h"
#include "graphic/sdl_graphics_manager.h"
#include "resource/resource_manager.h"
#include "scene/scene_manager.h"
#include "util/settings_manager.h"

using namespace std;
using namespace gamelib;

shared_ptr<game_world_data> game_world(new game_world_data); // shared game world
shared_ptr<global_config> config(new global_config); // game settings
shared_ptr<event_manager> event_admin(new event_manager(config)); // co-ordinates all events within the game
shared_ptr<sdl_graphics_manager> graphics_admin(new sdl_graphics_manager(event_admin)); // deals with our drawing operations
shared_ptr<audio_manager> audio_admin(new audio_manager); // makes audio resources
shared_ptr<font_manager> font_admin(new font_manager); // makes font resources
shared_ptr<resource_manager> resource_admin(new resource_manager(config, graphics_admin, font_admin, audio_admin )); // co-ordinates the resources in the game - such as holding definitions of all the resources/assets in the game
shared_ptr<scene_manager> scene_admin(new scene_manager(event_admin, config, resource_admin )); // co-ordinates the scene-related activities in the game
shared_ptr<game_structure> game(new game_structure(event_admin, resource_admin, config, game_world, scene_admin, graphics_admin)); // logical structure of the game such as the game initialization, game loop etc.
shared_ptr<settings_manager> settings_admin(new settings_manager);

int main(int argc, char *args[])
{
	try
	{
		const auto is_initialized = succeeded(game->initialize(), "Initialize");
		const auto is_loaded = succeeded(game->load_content(), "load_content");

		if(!is_initialized || !is_loaded)
			return -1;

		const auto loop_result = run_and_log("Beginning game loop...", config->verbose, [&]()
		{
			return game->game_loop();
		});

		const auto unload_result = run_and_log("Unloading game...", config->verbose, [&]()
		{
			return game->unload();
		});
		
		return succeeded(loop_result, "game loop failed") && 
			   succeeded(unload_result, "content unload failed");
	
	}
	catch(game_exception& game_exception)
	{
		log_message("Game Exception occurred from " + string(game_exception.get_source_subsystem()) + string(": ") + string(game_exception.what()));
	}
	return 0;
}
