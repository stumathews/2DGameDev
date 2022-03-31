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
		LevelManager levels;

		// Read game settings
		SettingsManager::Get()->Load("game/settings.xml");

		// Read verbosity setting
		const auto beVerbose = SettingsManager::Get()->GetBool("global", "verbose");

		// Create game infrastructure
		GameStructure game = GameStructure([&]() { levels.GetKeyboardInput(); });
		
		// Initialize key parts of the game
		const auto isGameStructureInitialized = IsSuccess(game.InitializeGameSubSystems(), "Initialize Game subsystems...");
		const auto IsLevelsInitialized = IsSuccess(levels.Initialize(), "Initializing Level Manager...");
		
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
			LogMessage(message, beVerbose, true);
			return -1;
		}

		

		// Start main game Loop		
		const auto loop_result = LogThis("Beginning game loop...", beVerbose, [&]() { return game.DoGameLoop(); }, true, true);
		
		// Game loop ended, start unloading the game...		
		const auto unload_result = LogThis("Unloading game...", beVerbose, [&]() { return game.UnloadGameSubsystems(); }, true, true);
		
		return IsSuccess(loop_result, "Game loop failed") && 
			   IsSuccess(unload_result, "Content unload failed");
	
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
