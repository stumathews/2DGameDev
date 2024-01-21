#include <SDL.h>
#include <cppgamelib/net.h>
#include "LevelManager.h"
#include "structure/GameStructure.h"
#include <GameDataManager.h>

#include "EnemyMovedEvent.h"
#include "EventNumber.h"
#include "ToolWindow.h"
#include "graphic/Subscribable.h"
#include "structure/FixedStepGameLoop.h"
#include "EventTap.h"
#include "events/AddGameObjectToCurrentSceneEvent.h"
#include "events/SubscriberHandledEvent.h"

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

void InitializeGameSubSystems(GameStructure& gameStructure);
void PrepareFirstLevel();
shared_ptr<FixedStepGameLoop> CreateGameLoopStrategy();
void GetInput(unsigned long deltaMs);



int main(int, char *[])
{
	GameDataManager::Get()->Initialize(false);
	ErrorLogManager::GetErrorLogManager()->Create("GameErrors.txt");

	try
	{
		GameStructure infrastructure(CreateGameLoopStrategy());
		
		InitializeGameSubSystems(infrastructure);

		// Register a Window to show causal relationships and events
		auto toolWindow = libcausality::ToolWindow("ToolWindow1", 340,340, "This is a Tool Window Title");

		// Tap into the fetchPickupEvent to track relationships between player and pickups
		EventManager::Get()->SetEventTap([](const shared_ptr<Event>& event, const IEventSubscriber* subscriber)
		{
			if(event->Id == PlayerMovedEventTypeEventId) return;
			if(event->Id == AddGameObjectToCurrentSceneEventId) return;
			if(event->Id == EnemyMovedEventId) return;
			if(event->Id == DrawCurrentSceneEventId) return;
			if(event->Id == UpdateAllGameObjectsEventTypeEventId) return;
			if(event->Id == UpdateProcessesEventId) return;
			if(event->Id == ControllerMoveEventId) return;

			libcausality::EventTap::Get()->Tap(event, const_cast<IEventSubscriber*>(subscriber)->GetSubscriberName(), GameDataManager::Get()->GameWorldData.ElapsedGameTime);
		});

		// Load level and create/add game objects
		PrepareFirstLevel();

		// Start the game loop which will pump update/draw events onto the event system, which level objects subscribe to
		infrastructure.DoGameLoop(&GameDataManager::Get()->GameWorldData);

		return IsSuccess(infrastructure.Unload(), "Unloading game subsystems successful.");	
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

	
	const auto initialized1 = gameStructure.Initialize(screenWidth, screenHeight, windowTitle, resourcesFilePath,
	                                                   gameSettingsFilePath);
	const auto initialized2 = LevelManager::Get()->Initialize();

	const auto someInitFailed = !IsSuccess(initialized1, "Successfully initialized game structure.") || 
								     !IsSuccess(initialized2, "Successfully initialized Level Manager...");

	if (someInitFailed)
	{
		const auto verboseLoggingEnabled = Settings::Bool("global", "verbose");

		LogMessage("Game subsystem initialization failed.", verboseLoggingEnabled, true);

		THROW(12, "There was a problem initializing the game subsystems", "Initialize Subsystems");
	}
}

void Update(const unsigned long deltaMs)
{	
	EventManager::Get()->ProcessAllEvents(deltaMs);
	EventManager::Get()->DispatchEventToSubscriber(EventFactory::Get()->CreateUpdateAllGameObjectsEvent(), deltaMs);
	EventManager::Get()->DispatchEventToSubscriber(EventFactory::Get()->CreateUpdateProcessesEvent(), deltaMs);
}

void Draw()
{
	 // Time-sensitive, skip queue. Draws the current scene
	EventManager::Get()->DispatchEventToSubscriber(EventFactory::Get()->CreateGenericEvent(DrawCurrentSceneEventId, "Game"), 0UL);
}

void GetInput(const unsigned long deltaMs)
{
	LevelManager::Get()->GetInputManager()->Sample(deltaMs);
	NetworkManager::Get()->Listen();	                                           
}

shared_ptr<FixedStepGameLoop> CreateGameLoopStrategy()
{
	return std::make_shared<FixedStepGameLoop>(16, Update, Draw, GetInput);
}

