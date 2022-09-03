#include <SDL.h>
#include <net/NetworkManager.h>
#include <Windows.h>
#include "LevelManager.h"
#include "GameStructure.h"
#include "game.h"
#include "Logging/ErrorLogManager.h"

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
	ErrorLogManager::GetErrorLogManager()->Create("GameErrors.txt");

	try
	{
		// Create our game structure that uses the level manager's polling function for keyboard input
		GameStructure infrastructure([&]() { LevelManager::Get()->GetKeyboardInput(); });
				
		// Initialize all the required game sub systems
		if (InitializeGameSubSystems(infrastructure)) 
			return -1;

		// Prepare the level before starting the game loop
		PrepareLevel();

		// Start the game loop which will pump update/draw events onto the event system, which certain objects subscribe to
		return IsSuccess(infrastructure.DoGameLoop(), "Game loop failed") && 
			   IsSuccess(infrastructure.UnloadGameSubsystems(), "Content unload failed");	
	}
	catch(EngineException& e)
	{
		MessageBoxA(nullptr, e.what(), "Game Error", MB_OK);
		
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
	if (!SceneManager::Get()->GetGameWorld().IsNetworkGame)
	{
		if (SettingsManager::Get()->GetBool("global", "createAutoLevel"))
		{
			LevelManager::Get()->CreateAutoLevel();
		}
		else
		{
			LevelManager::Get()->CreateLevel(SettingsManager::Get()->GetString("global","level1FileName"));
		}
	}

	Logger::Get()->LogThis(SceneManager::Get()->GetGameWorld().IsNetworkGame
		? NetworkManager::Get()->IsGameServer()
			? "Waiting for network players. Press 'n' to start network game."
			: "Waiting for the server to start the network game to begin." 
		: "Creating Single player level...");
}

int InitializeGameSubSystems(gamelib::GameStructure& infrastructure)
{
	const auto screenWidth = 0; // 0 will mean it will get read from the settings file
	const auto screenHeight = 0; // 0 will mean it will get read from the settings file
	if (!IsSuccess(infrastructure.InitializeGameSubSystems(screenWidth, screenHeight, "Mazer 2d"), "Initialize Game subsystems...") ||
		!IsSuccess(LevelManager::Get()->Initialize(), "Initializing Level Manager..."))
	{
		LogMessage("Game initialization failed.", SettingsManager::Get()->GetBool("global", "verbose"), true);
		return -1;
	}
	return 0;
}
