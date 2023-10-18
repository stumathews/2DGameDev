#include <SDL.h>
#include <net/NetworkManager.h>
#include <Windows.h>
#include "LevelManager.h"
#include "GameStructure.h"
#include "game.h"
#include "Logging/ErrorLogManager.h"
#include <GameDataManager.h>

using namespace std;
using namespace gamelib;
typedef SettingsManager Settings;
/*
  	##   ##    ##     ### ##   ### ###  ### ##            ## ##    ### ##
  	## ##      ##    ##  ##    ##  ##   ##  ##           ##  ##    ##  ##
  	# ### #   ## ##      ##     ##       ##  ##               ##    ##  ##  
  	## # ##   ##  ##    ##      ## ##    ## ##               ##     ##  ##
  	##   ##   ## ###   ##       ##       ## ##              ##      ##  ##
  	##   ##   ##  ##  ##  ##    ##  ##   ##  ##            #   ##   ##  ##
  	##   ##  ###  ##  # ####   ### ###  #### ##           ######   ### ##
*/


int main(int argc, char *args[])
{
	GameDataManager::Get()->Initialize();
	ErrorLogManager::GetErrorLogManager()->Create("GameErrors.txt");

	try
	{
		// Create our game structure that uses the level manager's polling function for keyboard input
		GameStructure infrastructure([&]() { LevelManager::Get()->GetKeyboardInput(); });
				
		InitializeGameSubSystems(infrastructure);

		// Load level and create/add game objects
		PrepareFirstLevel();

		// Start the game loop which will pump update/draw events onto the event system, which level objects subscribe to
		infrastructure.DoGameLoop(GameData::Get());

		return IsSuccess(infrastructure.Unload(), "Unloading game subsystems failed");	
	}
	catch(EngineException& e)
	{
		MessageBoxA(nullptr, e.what(), "Game Error!", MB_OK);
		
		ErrorLogManager::GetErrorLogManager()->Buffer << "****ERROR****\n";
		ErrorLogManager::GetErrorLogManager()->Flush();
		ErrorLogManager::GetErrorLogManager()->LogException(e);
		ErrorLogManager::GetErrorLogManager()->Buffer << "*************\n";
		ErrorLogManager::GetErrorLogManager()->Flush();
	}
	return 0;
}

void PrepareFirstLevel()
{
	const auto isSinglePlayerGame = GameData::Get()->IsSinglePlayerGame();

	if (isSinglePlayerGame)
	{
		auto _ = LevelManager::Get()->ChangeLevel(1);
		
		if (Settings::Bool("global", "createAutoLevel")) 
		{
			LevelManager::Get()->CreateAutoLevel();
		}
		else 
		{
			// Get the name of the level file for the first level
			const auto firstLevelFilePath = Settings::String("global", "level1FileName");

			LevelManager::Get()->CreateLevel(firstLevelFilePath); 
		}
	}

	Logger::Get()->LogThis(!isSinglePlayerGame
		? NetworkManager::Get()->IsGameServer()
			? "Waiting for network players. Press 'n' to start network game."
			: "Waiting for the server to start the network game to begin." 
		: "Creating Single player level...");
}

void InitializeGameSubSystems(GameStructure& gameStructure)
{
	constexpr auto screenWidth = 0; // 0 will mean it will get read from the settings file
	constexpr auto screenHeight = 0; // 0 will mean it will get read from the settings file
	constexpr auto windowTitle = "Mazer2D!";
	constexpr auto resourcesFilePath = "game\\Resources.xml";
	constexpr auto gameSettingsFilePath = "game/settings.xml";

	const auto initialized1 = gameStructure.Initialize(screenWidth, screenHeight, windowTitle, resourcesFilePath, gameSettingsFilePath);
	const auto initialized2 = LevelManager::Get()->Initialize();

	const auto someInitFailed = !IsSuccess(initialized1, "InitializeGameSubSystems...") || 
								 !IsSuccess(initialized2, "Initializing Level Manager...");

	if (someInitFailed)
	{
		const auto verboseLoggingEnabled = Settings::Bool("global", "verbose");

		LogMessage("Game initialization failed.", verboseLoggingEnabled, true);

		THROW(12, "There was a problem initializing the game subsystems", "Initialize Subsystems")
	}
}
