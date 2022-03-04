#include <SDL.h>
#include <Windows.h>
#include "LevelManager.h"

using namespace std;
using namespace gamelib;

int main(int argc, char *args[])
{
	try
	{
		// Main game Objects are created here and passed around as shared pointers throughout the game
		const auto world = make_shared<GameWorld>();		
		const auto settings = make_shared<settings_manager>();
		const auto events = make_shared<event_manager>(settings);
		const auto graphics = make_shared<sdl_graphics_manager>(events);
		const auto audio = make_shared<audio_manager>();
		const auto fonts = make_shared<font_manager>();
		const auto resources = make_shared<resource_manager>(settings, graphics, fonts, audio); 
		const auto currentScene = make_shared<scene_manager>(events, settings, resources, world);		
		const auto levels = make_shared<LevelManager>(events, resources, settings, world, currentScene, audio);

		// Read game settings
		settings->load("game/settings.xml");
		const auto beVerbose = settings->get_bool("global", "verbose");
		
		// Keyboard input polling function
		auto GetKeyboardInput = [&](){ levels->GetKeyboardInput(); };

		// Create game infrastructure
		const auto structure =  make_shared<game_structure>(events, resources, settings, world, currentScene, graphics, audio, GetKeyboardInput );
		
		// Initialize key parts of the game
		const auto isGameStructureInitialized = succeeded(structure->initialize(), "Initialize Game...");
		const auto IsLevelsInitialized = succeeded(levels->initialize(), "Initializing Level Manager...");
				
		// Create the level
		levels->CreateLevel();

		// Abort game if initialization failed
		if(!isGameStructureInitialized || !IsLevelsInitialized)
		{
			auto message = string("Game initialization failed.") + 
				           string("GameStructured initialized=") + std::to_string(isGameStructureInitialized) + 
				           string(" LevelInitialized=") + std::to_string(IsLevelsInitialized);
			log_message(message);
			return -1;
		}

		// Start main game Loop		
		const auto loop_result = run_and_log("Beginning game loop...", beVerbose, [&]() { return structure->game_loop(); }, true, true, settings);

		// ** Game loop finished ** //
		
		// Start unloading the game		
		const auto unload_result = run_and_log("Unloading game...", beVerbose, [&]() { return structure->unload(); }, true, true, settings);
		
		return succeeded(loop_result, "Game loop failed") && succeeded(unload_result, "Content unload failed");
	
	}
	catch(game_exception& game_exception)
	{
		log_message("Game Exception occurred from " + string(game_exception.get_source_subsystem()) + string(": ") + string(game_exception.what()));
	}

	return 0;
}
