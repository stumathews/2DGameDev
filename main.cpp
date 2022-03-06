#include <SDL.h>
#include <Windows.h>
#include "LevelManager.h"

using namespace std;
using namespace gamelib;

int main(int argc, char *args[])
{
	try
	{
		// Main game Objects are created here
		auto world = GameWorld();		
		auto gameLogger = Logger();
		auto settings = SettingsManager();
		auto events = EventManager(settings, gameLogger);
		auto graphics = sdl_graphics_manager(events, gameLogger);                                       
		auto audio = AudioManager();
		auto fonts = FontManager();
		auto resources = ResourceManager(settings, graphics, fonts, audio, gameLogger);                                   
		auto currentScene = SceneManager(events, settings, resources, world, gameLogger);		
		auto levels = LevelManager(events, resources, settings, world, currentScene, audio, gameLogger);

		// Read game settings
		settings.load("game/settings.xml");
		const auto beVerbose = settings.get_bool("global", "verbose");
		
		// Keyboard input polling function
		auto GetKeyboardInput = [&](){ levels.GetKeyboardInput(); };

		// Create game infrastructure
		const auto structure =  shared_ptr<GameStructure>(new GameStructure(events, resources, settings, world, currentScene, graphics, audio, GetKeyboardInput, gameLogger));
		
		// Initialize key parts of the game
		const auto isGameStructureInitialized = succeeded(structure->initialize(), "Initialize Game...");
		const auto IsLevelsInitialized = succeeded(levels.initialize(), "Initializing Level Manager...");
		
		// Create the level
		levels.CreateLevel();

		// Abort game if initialization failed
		if(!isGameStructureInitialized || !IsLevelsInitialized)
		{
			auto message = string("Game initialization failed.") + 
				           string("GameStructured initialized=") + std::to_string(isGameStructureInitialized) + 
				           string(" LevelInitialized=") + std::to_string(IsLevelsInitialized);
			log_message(message, gameLogger, beVerbose, true);
			return -1;
		}

		// Start main game Loop		
		const auto loop_result = run_and_log("Beginning game loop...", beVerbose, [&]() { return structure->game_loop(); }, settings, true, true);

		// ** Game loop finished ** //
		
		// Start unloading the game		
		const auto unload_result = run_and_log("Unloading game...", beVerbose, [&]() { return structure->unload(); }, settings, true, true);
		
		return succeeded(loop_result, "Game loop failed") && succeeded(unload_result, "Content unload failed");
	
	}
	catch(game_exception& game_exception)
	{
		cout << "Game Exception occurred from " + string(game_exception.get_source_subsystem()) + string(": ") + string(game_exception.what()) << endl;
	}

	return 0;
}
