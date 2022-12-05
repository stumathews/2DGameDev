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

	// Ready the game log
	ErrorLogManager::GetErrorLogManager()->Create("GameErrors.txt");

	try
	{
		// Create our game structure that uses the level manager's polling function for keyboard input
		GameStructure infrastructure([&]() 
			{
				// We will inject our custom controller-input function into the game structure
				LevelManager::Get()->GetKeyboardInput(); 
			});
				
		// Initialize all the required game sub systems
		if (InitializeGameSubSystems(infrastructure)) 
			THROW(12, "There was a problem initializing the game subsystems","Initialize Subsystems");

		// Load level and create/add game objects
		PrepareLevel();

		// Start the game loop which will pump update/draw events onto the event system, which level objects subscribe to
		return IsSuccess(infrastructure.DoGameLoop(GameData::Get()), "Game loop failed") &&
			   // Then unload the game subsystems
			   IsSuccess(infrastructure.UnloadGameSubsystems(), "Content unload failed");	
	}
	catch(EngineException& e)
	{
		// Something went wrong trying to start/setup the game!
		MessageBoxA(nullptr, e.what(), "Game Error!", MB_OK);
		
		ErrorLogManager::GetErrorLogManager()->Buffer << "****ERROR****\n";
		ErrorLogManager::GetErrorLogManager()->Flush();
		ErrorLogManager::GetErrorLogManager()->LogException(e);
		ErrorLogManager::GetErrorLogManager()->Buffer << "*************\n";
		ErrorLogManager::GetErrorLogManager()->Flush();
	}

	return 0;
}

void PrepareLevel()
{
	// Single player mode
	if (!GameData::Get()->IsNetworkGame)
	{
		LevelManager::Get()->ChangeLevel(1);
		
		if (SettingsManager::Get()->GetBool("global", "createAutoLevel")) 
		{
			LevelManager::Get()->CreateAutoLevel();
		}
		else 
		{ 
			LevelManager::Get()->CreateLevel(SettingsManager::Get()->GetString("global", "level1FileName")); 
		}
	}

	Logger::Get()->LogThis(GameData::Get()->IsNetworkGame
		? NetworkManager::Get()->IsGameServer()
			? "Waiting for network players. Press 'n' to start network game."
			: "Waiting for the server to start the network game to begin." 
		: "Creating Single player level...");
}

int InitializeGameSubSystems(gamelib::GameStructure& infrastructure)
{
	const auto screenWidth = 0; // 0 will mean it will get read from the settings file
	const auto screenHeight = 0; // 0 will mean it will get read from the settings file	

	if (!IsSuccess(infrastructure.InitializeGameSubSystems(screenWidth, screenHeight, "Initialize Game subsystems...", 
		"game\\Resources.xml"), "InitializeGameSubSystems...") ||
		!IsSuccess(LevelManager::Get()->Initialize(), "Initializing Level Manager..."))
	{
		LogMessage("Game initialization failed.", SettingsManager::Get()->GetBool("global", "verbose"), true);
		return -1;
	}
	return 0;
}
