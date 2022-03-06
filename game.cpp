#include <SDL.h>
#include <Windows.h>
#include "LevelManager.h"
#include "GameStructure.h"
#include "game.h"
#include "Logging/ErrorLogManager.h"

using namespace std;
using namespace gamelib;


int main(int argc, char *args[])
{
	auto logManager = ErrorLogManager::GetErrorLogManager();
	logManager->Create("GameErrors.txt");

	try
	{
		// Main game Objects are created here
		GameWorld gameWorld;		
		Logger logger;
		SettingsManager settings;
		AudioManager audio;
		FontManager fonts;
		EventManager events = EventManager(settings, logger);
		SDLGraphicsManager graphics = SDLGraphicsManager(events, logger);		
		ResourceManager resources = ResourceManager(settings, graphics, fonts, audio, logger);
		SceneManager currentScene = SceneManager(events, settings, resources, gameWorld, logger);
		LevelManager levels = LevelManager(events, resources, settings, gameWorld, currentScene, audio, logger);

		// Read game settings
		settings.load("game/settings.xml");
		const auto beVerbose = settings.get_bool("global", "verbose");

		// Create game infrastructure
		GameStructure game = GameStructure(events, resources, settings, gameWorld, currentScene, graphics, audio,
			// The level manager is responsible for polling for input
			[&]() { levels.GetKeyboardInput(); },
			logger);
		
		// Initialize key parts of the game
		const auto isGameStructureInitialized = IsSuccess(game.InitializeGameSubSystems(), "Initialize Game subsystems...", logger);
		const auto IsLevelsInitialized = IsSuccess(levels.Initialize(), "Initializing Level Manager...", logger);
		
		// Sets up level becuase we want these events to be in the queue before the level events
		levels.PrepareLevel(1);

		// Create the level
		levels.CreateLevel();

		// Abort game if initialization failed
		if(!isGameStructureInitialized || !IsLevelsInitialized)
		{
			auto message = string("Game initialization failed.") + 
				           string("GameStructured initialized=") + std::to_string(isGameStructureInitialized) + 
				           string(" LevelInitialized=") + std::to_string(IsLevelsInitialized);
			LogMessage(message, logger, beVerbose, true);
			return -1;
		}

		

		// Start main game Loop		
		const auto loop_result = LogThis("Beginning game loop...", beVerbose, [&]() { return game.DoGameLoop(); }, settings, true, true);
		
		// Game loop ended, start unloading the game...		
		const auto unload_result = LogThis("Unloading game...", beVerbose, [&]() { return game.UnloadGameSubsystems(); }, settings, true, true);
		
		return IsSuccess(loop_result, "Game loop failed", logger) && 
			   IsSuccess(unload_result, "Content unload failed", logger);
	
	}
	catch(EngineException& e)
	{
		MessageBoxA(nullptr, e.what(), "", MB_OK);
		
		logManager->Buffer << "****ERROR****\n";
		logManager->Flush();
		logManager->LogException(e);
		logManager->Buffer << "*************\n";
		logManager->Flush();

	}

	return 0;
}
