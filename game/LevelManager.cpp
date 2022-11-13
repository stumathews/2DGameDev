#include "LevelManager.h"
#include "common/Common.h"
#include "Pickup.h"
#include "common/constants.h"
#include "events/AddGameObjectToCurrentSceneEvent.h"
#include "RoomGenerator.h"
#include "objects/game_world_component.h"
#include <events/GameObjectEvent.h>
#include "Player.h"
#include <memory>
#include <events/SceneChangedEvent.h>
#include <objects/GameObjectFactory.h>
#include "PlayerMoveStrategy.h"
#include "GameObjectEventFactory.h"
#include <GameData.h>
#include <sstream>
#include <events/NetworkPlayerJoinedEvent.h>
#include <events/StartNetworkLevelEvent.h>
#include <Level.h>
#include <Rooms.h>
#include <objects/StaticSprite.h>
#include <events/UpdateAllGameObjectsEvent.h>
#include <events/SceneChangedEvent.h>
#include "util/RectDebugging.h"
#include "windows.h"
#include <events/UpdateProcessesEvent.h>
#include <processes/DelayProcess.h>
#include <processes/Action.h>
#include <functional>

using namespace gamelib;
using namespace std;

bool LevelManager::Initialize()
{
	// Initialize game world data
	SceneManager::Get()->GetGameWorld().IsGameDone = false;
	SceneManager::Get()->GetGameWorld().IsNetworkGame = false;
	SceneManager::Get()->GetGameWorld().CanDraw = true;

	// easy access to subscribing and raising events
	eventManager = EventManager::Get();

	// easy way to create known events
	eventFactory = EventFactory::Get();

	// level manager translates input into game commands
	_gameCommands = shared_ptr<GameCommands>(new GameCommands());

	eventManager->SubscribeToEvent(EventType::GenerateNewLevel, this);
	eventManager->SubscribeToEvent(EventType::InvalidMove, this);
	eventManager->SubscribeToEvent(EventType::FetchedPickup, this);
	eventManager->SubscribeToEvent(EventType::GameObject, this);
	eventManager->SubscribeToEvent(EventType::LevelChangedEventType, this);
	eventManager->SubscribeToEvent(EventType::NetworkPlayerJoined, this);
	eventManager->SubscribeToEvent(EventType::StartNetworkLevel, this);
	eventManager->SubscribeToEvent(EventType::UpdateProcesses, this);

	verbose = SettingsManager::Get()->GetBool("global", "verbose");

	// Determine if the settings indicate if we should start in network game mode
	SceneManager::Get()->GetGameWorld().IsNetworkGame = SettingsManager::Get()->GetBool("global", "isNetworkGame");
			
	return true;
}

gamelib::ListOfEvents LevelManager::HandleEvent(std::shared_ptr<Event> event)
{
	gamelib::ListOfEvents newEvents;
	std::shared_ptr<GameObjectEvent> gameObjectEvent = nullptr;
	std::stringstream output;
	
	switch(event->type)
	{
	case EventType::UpdateProcesses:
		{
			processManager.UpdateProcesses(dynamic_pointer_cast<UpdateProcessesEvent>(event)->deltaMs);
		}
		break;
	case EventType::InvalidMove:
		_gameCommands->InvalidMove();
		break;
	case EventType::FetchedPickup:
		_gameCommands->FetchedPickup();
		hudItem->AdvanceFrame();
		if (numLevelPickups == 0)
		{
			auto toggleMusicOffProcess = std::shared_ptr<Action>(new Action([&]() { _gameCommands->ToggleMusic(verbose); }));
			auto playVictoryMusicProcess = std::shared_ptr<Action>(new Action([&]() { AudioManager::Get()->Play(AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo(SettingsManager::Get()->GetString("audio", "win_music")))->AsSoundEffect()); }));
			auto loadNextLevelProcess = std::shared_ptr<Action>(new Action([&]() { currentLevel = currentLevel < 4 ? ++currentLevel : 1; _gameCommands->LoadNewLevel(currentLevel); }));
			auto delayFive = std::shared_ptr<Process>(new DelayProcess(5000));
			
			toggleMusicOffProcess->AttachChild(playVictoryMusicProcess);
			playVictoryMusicProcess->AttachChild(delayFive);
			delayFive->AttachChild(loadNextLevelProcess);

			processManager.AttachProcess(toggleMusicOffProcess);
			Logger::Get()->LogThis("All Pickups Collected Well Done!");
			
		}		
		break;		
	case EventType::GenerateNewLevel:
		GenerateNewLevel();
		break;
	case EventType::LevelChangedEventType:
		OnLevelChanged(event);
		break;
	case EventType::GameObject:
		gameObjectEvent = dynamic_pointer_cast<GameObjectEvent>(event);
		switch(gameObjectEvent->context)
		{
			case GameObjectEventContext::Remove:
				RemoveGameObject(*gameObjectEvent->gameObject);
				break;
			default:				
				output << "Unknown Game Object Event:" << ToString(gameObjectEvent->context);
				gamelib::Logger::Get()->LogThis(output.str());
		}
		break;
	case EventType::NetworkPlayerJoined:
		OnNetworkPlayerJoined(event);
		break;
	case EventType::StartNetworkLevel:
		OnStartNetworkLevel(event);
		break;
	}

	return newEvents;
}


void LevelManager::OnStartNetworkLevel(std::shared_ptr<gamelib::Event> evt)
{
	// read the start level event and create the level
	ChangeLevel(1);

	// Network games all start on level 1 for now
	CreateLevel(SettingsManager::Get()->GetString("global", "level1FileName"));
}

void LevelManager::OnNetworkPlayerJoined(std::shared_ptr<gamelib::Event> evt)
{
	// We know a player has joined the game server.
	auto networkPlayerJoinedEvent = dynamic_pointer_cast<NetworkPlayerJoinedEvent>(evt);
	auto player = networkPlayerJoinedEvent->player;
	// Add the player to our level... find a suitable position for it
}

void LevelManager::GenerateNewLevel()
{
	RemoveAllGameObjects();
	CreateAutoLevel();
}

void LevelManager::RemoveAllGameObjects()
{
	auto& objects = SceneManager::Get()->GetGameWorld().GetGameObjects();
	std::for_each(std::begin(objects), std::end(objects), [this](std::shared_ptr<gamelib::GameObject> gameObject)
	{
		eventManager->RaiseEvent(GameObjectEventFactory::MakeRemoveObjectEvent(&(*gameObject)), this);
	});

	//Reclassify game objects

	GameData::Get()->SetGameObjects(&SceneManager::Get()->GetGameWorld().GetGameObjects());
}

void LevelManager::OnLevelChanged(std::shared_ptr<gamelib::Event>& evt)
{
	const auto levelChangedEvent = dynamic_pointer_cast<SceneChangedEvent>(evt);
	auto level = levelChangedEvent->scene_id;

	switch(level)
	{
		case 1:
			PlayLevelMusic("LevelMusic1");
			break;
		case 2:
			PlayLevelMusic("LevelMusic2");
			break;
		case 3:
			PlayLevelMusic("LevelMusic3");
			break;
		case 4:
			PlayLevelMusic("LevelMusic4");
			break;
		default:
			std::stringstream message;
			message << "Unexpected level " << level;
			Logger::Get()->LogThis(message.str());
	}
}

void LevelManager::PlayLevelMusic(std::string levelMusicAssetName)
{
	auto asset = ResourceManager::Get()->GetAssetInfo(levelMusicAssetName);
	if (asset->isLoadedInMemory)
	{
		if (asset && asset->isLoadedInMemory)
		{
			AudioManager::Get()->Play(gamelib::AudioManager::ToAudioAsset(asset)->AsMusic());
		}
	}
}

std::string LevelManager::GetSubscriberName() 
{ 
	return "level_manager";
};

void LevelManager::RemoveGameObject(gamelib::GameObject& gameObject)
{
	auto& gameObjects = SceneManager::Get()->GetGameWorld().GetGameObjects();
	
	// Look for gameObject
	auto findResult = std::find_if(begin(gameObjects), end(gameObjects), [&](weak_ptr<gamelib::GameObject> target)
	{ 
		if(auto underlyingGameObject = target.lock()) // Test to see if the game object reference still exists 
		{
			// Found object we're looking for?
			return underlyingGameObject->Id == gameObject.Id;
		}
		return false;
	});
	
	auto found = findResult != end(gameObjects);
			
	if(found)
	{
		eventManager->Unsubscribe((*findResult)->Id);
		gameObjects.erase(findResult);
	}

	GameData::Get()->SetGameObjects(&gameObjects);
}

void LevelManager::GetKeyboardInput()
{
	auto keyState = SDL_GetKeyboardState(NULL);
	
	// continuous-response keys
	if (keyState[SDL_SCANCODE_UP] || keyState[SDL_SCANCODE_W])
	{
		_gameCommands->MoveUp(verbose);
	}
	if (keyState[SDL_SCANCODE_DOWN] || keyState[SDL_SCANCODE_S])
	{
		_gameCommands->MoveDown(verbose);
	}
	if (keyState[SDL_SCANCODE_LEFT] || keyState[SDL_SCANCODE_A])
	{
		_gameCommands->MoveLeft(verbose);
	}
	if (keyState[SDL_SCANCODE_RIGHT] || keyState[SDL_SCANCODE_D])
	{
		_gameCommands->MoveRight(verbose);
	}

	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent))
	{		
		// single-hit keys, mouse and other general SDL events (eg. windowing)
		if (sdlEvent.type == SDL_KEYDOWN)
		{
			switch (sdlEvent.key.keysym.sym)
			{
			case SDLK_SPACE:
				_gameCommands->Fire(verbose);
				break;			
			case SDLK_q:
			case SDLK_ESCAPE:
				_gameCommands->Quit(verbose);
				break;
			case SDLK_h:	
				_gameCommands->LoadNewLevel(1);
				currentLevel = 1;
				break;
			case SDLK_j:
				_gameCommands->LoadNewLevel(2);
				currentLevel = 2;
				break;
			case SDLK_k:
				_gameCommands->LoadNewLevel(3);
				currentLevel = 3;
				break;
			case SDLK_l:
				_gameCommands->LoadNewLevel(4);
				currentLevel = 4;
				break;
			case SDLK_1:
				_gameCommands->PlaySoundEffect(gamelib::AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo("high.wav"))->AsSoundEffect());				
				break;
			case SDLK_2:
				_gameCommands->PlaySoundEffect(gamelib::AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo("medium.wav"))->AsSoundEffect());
				break;
			case SDLK_3:
				_gameCommands->PlaySoundEffect(gamelib::AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo("low.wav"))->AsSoundEffect());
				break;
			case SDLK_4:
				_gameCommands->PlaySoundEffect(gamelib::AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo("scratch.wav"))->AsSoundEffect());
				break;
			case SDLK_9:
				_gameCommands->ToggleMusic(verbose);
				break;
			case SDLK_r:				
				_gameCommands->ReloadSettings(verbose);
				break;
			case SDLK_g:				
				_gameCommands->GenerateNewLevel(verbose);
				break;
			case SDLK_p:
				_gameCommands->PingGameServer();
				break;
			case SDLK_n:
				_gameCommands->StartNetworkLevel();				
				break;
			}
		}
		
		if (sdlEvent.type == SDL_QUIT)
		{
			_gameCommands->Quit(verbose);
			return;
		}
	}
}


size_t LevelManager::GetRandomIndex(const int min, const int max) 
{ 
	return rand() % (max - min + 1) + min;
}

shared_ptr<Player> LevelManager::CreatePlayer(const vector<shared_ptr<Room>> rooms, const int playerWidth, const int playerHeight) const
{		
	const auto playerRoomIndex = GetRandomIndex(0, rooms.size());
	
	// Automatically determine which room to place the player - random
	const auto playerRoom = rooms[playerRoomIndex];
	const auto centerPositionInRoom = playerRoom->GetCenter(playerWidth, playerHeight);	

	// Read the player's name from config file. In the future there could be a player xml definition
	const auto playerNickName = SceneManager::Get()->GetGameWorld().IsNetworkGame 
								? SettingsManager::Get()->GetString("networking", "nickname")
								: "Player1";

	// Create the player in room
	return shared_ptr<Player>(new Player(playerRoom, playerWidth, playerHeight, playerNickName));
}

ListOfGameObjects LevelManager::CreatePickups(const vector<shared_ptr<Room>>& rooms, const int pickupWidth, const int pickupHeight)
{
	ListOfGameObjects pickups;
	const auto numPickups = SettingsManager::Get()->GetInt("global", "numPickups");
	auto part = numPickups / 3;

	for(auto i = 0; i < numPickups; i++)
	{
		const auto rand_index = GetRandomIndex(0, rooms.size()-1);
		const auto random_room = rooms[rand_index];	
		const auto positionInRoom = random_room->GetCenter(pickupWidth, pickupHeight);

		auto pickup = std::shared_ptr<Pickup>(new Pickup(positionInRoom.GetX(), positionInRoom.GetY(), pickupWidth, pickupHeight, true));
		
		// Place 3 sets evently of each type of pickup	
		if(i < 1 * part) 
		{
			pickup->stringProperties["assetName"] = SettingsManager::Get()->GetString("pickup1", "assetName");		
		}
		else if(i >= 1 * part && i < 2 * part) 
		{ 
			pickup->stringProperties["assetName"] = SettingsManager::Get()->GetString("pickup2", "assetName"); 
		}
		else if(i >= 2 * part)
		{
			pickup->stringProperties["assetName"] = SettingsManager::Get()->GetString("pickup3", "assetName");
		}

		pickup->Initialize();
		pickup->RoomNumber = rand_index;
		pickups.push_back(pickup);
	}

	numLevelPickups = pickups.size();

	return pickups;
}

ListOfGameObjects LevelManager::CreateLevel(string filename)
{	
	RemoveAllGameObjects();

	// Load the level definition file
	level = std::shared_ptr<Level>(new Level(filename));	
	level->Load();
		
	// Determine the dimensions of the rows and columns based on the available spaces
	const auto rowWidth = SettingsManager::Get()->GetInt("global", "screen_width") / level->NumCols;
	const auto rowHeight = SettingsManager::Get()->GetInt("global", "screen_height") / level->NumRows;

	// Get reference to where all our game objects will be kept
	auto& gameObjectsPtr = SceneManager::Get()->GetGameWorld().GetGameObjects();
	
	// Get reference to the list of rooms in the level
	auto rooms = level->Rooms;

	InitializeRooms(rooms, gameObjectsPtr);	

	// Create the pickups
	const auto pickupWidth = rowWidth / 2;
	const auto pickupHeight = rowHeight / 2;
	const auto pickups = CreatePickups(rooms, pickupWidth, pickupHeight);

	// Create the player
	auto playerSpriteAsset = dynamic_pointer_cast<SpriteAsset>(ResourceManager::Get()->GetAssetInfo("edge_player"));
	const auto player = CreatePlayer(rooms, playerSpriteAsset->Dimensions.GetWidth(), playerSpriteAsset->Dimensions.GetHeight());
	
	InitializePlayer(player, playerSpriteAsset);
	RegisterGameObject(player);

	// Create Hud
	hudItem = StaticSprite::Create(rooms[rooms.size() - 1]->GetCenter(player->GetWidth(), player->GetHeight()), 
		dynamic_pointer_cast<gamelib::SpriteAsset>(ResourceManager::Get()->GetAssetInfo("hudspritesheet")));

	InitializeHudItem(hudItem);
	RegisterGameObject(hudItem);

	// Setup the pickups
	InitializePickups(pickups, gameObjectsPtr);
	
	// Add single UI elements to the scene
	AddGameObjectToScene(player);
	AddGameObjectToScene(hudItem);

	// Add the framerate
	drawableFrameRate = std::shared_ptr<DrawableFrameRate>(new DrawableFrameRate(&level->Rooms[level->Rooms.size()-2]->Bounds));
	AddGameObjectToScene(drawableFrameRate);
	RegisterGameObject(drawableFrameRate);

	GameData::Get()->SetGameObjects(&gameObjectsPtr);	

	return gameObjectsPtr;
}

void LevelManager::InitializeHudItem(std::shared_ptr<StaticSprite> hudItem)
{
	hudItem->LoadSettings();
	hudItem->SubscribeToEvent(EventType::PlayerMovedEventType);
}

void LevelManager::AddGameObjectToScene(std::shared_ptr<gamelib::GameObject> object)
{
	EventManager::Get()->RaiseEvent(std::dynamic_pointer_cast<gamelib::Event>(EventFactory::Get()->CreateAddToSceneEvent(object)), this);
};


ListOfGameObjects LevelManager::CreateAutoLevel()
{
	// Make sure our resources are findable
	ResourceManager::Get()->IndexResourceFile();

	// Register change level event
	LevelManager::Get()->ChangeLevel(1);

	// Calculate room info
	const auto rows = SettingsManager::Get()->GetInt("grid", "rows");
	const auto cols = SettingsManager::Get()->GetInt("grid", "cols");
	const auto screenWidth = SettingsManager::Get()->GetInt("global", "screen_width");
	const auto screenHeight = SettingsManager::Get()->GetInt("global", "screen_height");
	const auto removeRandomSides = SettingsManager::Get()->GetBool("grid", "removeSidesRandomly");
	const auto rowWidth = screenWidth / cols; 
	const auto rowHeight = screenHeight / rows;

	// Get reference to game objects
	auto& gameObjectsPtr = SceneManager::Get()->GetGameWorld().GetGameObjects();
	
	// Generate new rooms automatically	
	auto rooms = RoomGenerator(screenWidth, screenHeight, rows, cols, removeRandomSides).Generate();
	
	InitializeRooms(rooms, gameObjectsPtr);
			
	// Create the player
	
	auto playerSpriteAsset = dynamic_pointer_cast<SpriteAsset>(ResourceManager::Get()->GetAssetInfo("edge_player"));
	const auto player = CreatePlayer(rooms, playerSpriteAsset->Dimensions.GetWidth(), playerSpriteAsset->Dimensions.GetHeight());

	// Setup the player
	InitializePlayer(player, playerSpriteAsset);

	// Create the pickups
	const auto pickupWidth = 32;
	const auto pickupHeight = 32;
	const auto pickups = CreatePickups(rooms, pickupWidth, pickupHeight);

	// Setup the pickups
	InitializePickups(pickups, gameObjectsPtr);
	
	// Add player to game world
	RegisterGameObject(player);

	GameData::Get()->SetGameObjects(&gameObjectsPtr);
		
	return gameObjectsPtr;
}

void LevelManager::InitializePlayer(std::shared_ptr<Player> player, std::shared_ptr<gamelib::SpriteAsset> spriteAsset)
{
	// Set the player's control/movement strategy
	player->SetMoveStrategy(shared_ptr<PlayerMoveStrategy>(new PlayerMoveStrategy(player, 2)));
	player->SetTag(constants::playerTag);

	// Player can load its own settings
	player->LoadSettings();

	// Load/Create/Set the player's sprite
	player->SetSprite(AnimatedSprite::Create(player->Position.GetX(), player->Position.GetY(),
		spriteAsset));

	// We keep a reference to track of the player globally
	SceneManager::Get()->GetGameWorld().player = player;
}

void LevelManager::InitializePickups(const ListOfGameObjects& pickups, ListOfGameObjects& gameObjectsPtr)
{
	// Loop through each pickup and configure each one properly
	for (const auto& pickup : pickups)
	{
		pickup->LoadSettings();
		pickup->SubscribeToEvent(EventType::PlayerMovedEventType);

		// Register game object with the scene
		AddGameObjectToScene(pickup);

		// Keep track of this object as a member of all game objects
		RegisterGameObject(pickup);
	}
}

void LevelManager::InitializeRooms(std::vector<std::shared_ptr<Room>>& rooms, ListOfGameObjects& gameObjects)
{
	// Loop through each room and set each room up
	for (const auto& room : rooms)
	{
		room->LoadSettings();
		room->SubscribeToEvent(EventType::PlayerMovedEventType);
		room->SubscribeToEvent(EventType::SettingsReloaded);

		// Register game object in the scene
		AddGameObjectToScene(room);

		// Add a reference of this room as a member of the game objects
		gameObjects.push_back(dynamic_pointer_cast<gamelib::GameObject>(room));
	}
}

bool LevelManager::ChangeLevel(int level)
{
	bool isSuccess = false;
	try
	{
		_gameCommands->RaiseChangedLevel(false, level);
		isSuccess = true;
	}
	catch (...)
	{
		LogMessage("Could not start level for unknown reasons. level=" + std::to_string(level));
	}
	return isSuccess;
}

void LevelManager::RegisterGameObject(std::shared_ptr<GameObject> obj)
{
	SceneManager::Get()->GetGameWorld().GetGameObjects().push_back(obj);
}

int LevelManager::ReducePickupCount()
{
	return --numLevelPickups;
}

int LevelManager::IncreasePickupCount()
{
	return ++numLevelPickups;
}

shared_ptr<Level> LevelManager::GetLevel()
{
	return level;
}

LevelManager* LevelManager::Get()
{
	if (Instance == nullptr)
	{
		Instance = new LevelManager();
	}
	return Instance;
}

LevelManager::~LevelManager()
{
	Instance = nullptr;
}

LevelManager* LevelManager::Instance = nullptr;
