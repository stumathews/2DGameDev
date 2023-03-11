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
	ErrorLogManager::GetErrorLogManager()->Create("GameErrors.txt");

	try
	{
		// Create our game structure that uses the level manager's polling function for keyboard input
		GameStructure infrastructure([&]() { LevelManager::Get()->GetKeyboardInput(); });
				
		InitializeGameSubSystems(infrastructure);

		// Load level and create/add game objects
		PrepareLevel();

		// Start the game loop which will pump update/draw events onto the event system, which level objects subscribe to
		infrastructure.DoGameLoop(GameData::Get());

		return IsSuccess(infrastructure.UnloadGameSubsystems(), "Unloading game subsystems failed");	
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

void PrepareLevel()
{
	// Single player mode
	if (!GameData::Get()->IsNetworkGame)
	{
		// ReSharper disable once CppExpressionWithoutSideEffects
		// ReSharper disable once CppNoDiscardExpression
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

void InitializeGameSubSystems(GameStructure& infrastructure)
{
	// ReSharper disable once CppTooWideScopeInitStatement
	constexpr auto screenWidth = 0; // 0 will mean it will get read from the settings file
	// ReSharper disable once CppTooWideScopeInitStatement
	constexpr auto screenHeight = 0; // 0 will mean it will get read from the settings file	

	if (!IsSuccess(infrastructure.InitializeGameSubSystems(screenWidth, screenHeight, "Initialize Game subsystems...", 
		"game\\Resources.xml"), "InitializeGameSubSystems...") ||
		!IsSuccess(LevelManager::Get()->Initialize(), "Initializing Level Manager..."))
	{
		LogMessage("Game initialization failed.", SettingsManager::Get()->GetBool("global", "verbose"), true);
		THROW(12, "There was a problem initializing the game subsystems", "Initialize Subsystems")
	}
}
