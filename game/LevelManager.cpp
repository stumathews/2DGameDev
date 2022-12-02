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
	SceneManager::Get()->GetGameWorld().IsGameDone = false;
	SceneManager::Get()->GetGameWorld().IsNetworkGame = false;
	SceneManager::Get()->GetGameWorld().CanDraw = true;

	_eventManager = EventManager::Get();
	_eventFactory = EventFactory::Get();
	_gameCommands = shared_ptr<GameCommands>(new GameCommands());

	_eventManager->SubscribeToEvent(EventType::GenerateNewLevel, this);
	_eventManager->SubscribeToEvent(EventType::InvalidMove, this);
	_eventManager->SubscribeToEvent(EventType::FetchedPickup, this);
	_eventManager->SubscribeToEvent(EventType::GameObject, this);
	_eventManager->SubscribeToEvent(EventType::LevelChangedEventType, this);
	_eventManager->SubscribeToEvent(EventType::NetworkPlayerJoined, this);
	_eventManager->SubscribeToEvent(EventType::StartNetworkLevel, this);
	_eventManager->SubscribeToEvent(EventType::UpdateProcesses, this);

	_verbose = SettingsManager::Get()->GetBool("global", "verbose");

	SceneManager::Get()->GetGameWorld().IsNetworkGame = SettingsManager::Get()->GetBool("global", "isNetworkGame");
			
	return true;
}

gamelib::ListOfEvents LevelManager::HandleEvent(std::shared_ptr<Event> event, unsigned long deltaMs)
{
	switch(event->type)
	{
		case EventType::GenerateNewLevel: {	GenerateNewLevel(); } break;
		case EventType::LevelChangedEventType: { OnLevelChanged(event); } break;
		case EventType::UpdateProcesses: { processManager.UpdateProcesses(deltaMs); } break;
		case EventType::InvalidMove: { _gameCommands->InvalidMove(); } break;
		case EventType::NetworkPlayerJoined: { 	OnNetworkPlayerJoined(event); }	break;
		case EventType::StartNetworkLevel: { OnStartNetworkLevel(event); }	break;
		case EventType::FetchedPickup: { OnFetchedPickup(); } break;		
		case EventType::GameObject: { OnGameObjectEventReceived(event); } break;	
	}

	return gamelib::ListOfEvents();
}

void LevelManager::OnGameObjectEventReceived(std::shared_ptr<gamelib::Event>& event)
{	
	auto gameObjectEvent = dynamic_pointer_cast<GameObjectEvent>(event);
	switch (gameObjectEvent->context)
	{
		case GameObjectEventContext::Remove: { RemoveGameObject(*gameObjectEvent->gameObject); } break;
		default:
		{
			std::stringstream output;
			output << "Unknown Game Object Event:" << ToString(gameObjectEvent->context);
			gamelib::Logger::Get()->LogThis(output.str());
		}
	}
}

void LevelManager::OnFetchedPickup()
{
	_gameCommands->FetchedPickup();
	_hudItem->AdvanceFrame();

	if (numLevelPickups == 0)
	{
		auto a = std::shared_ptr<Action>(new Action([&]() { _gameCommands->ToggleMusic(_verbose); }));
		auto b = std::shared_ptr<Action>(new Action([&]() { AudioManager::Get()->Play(AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo(SettingsManager::Get()->GetString("audio", "win_music")))->AsSoundEffect()); }));
		auto d = std::shared_ptr<Action>(new Action([&]() { currentLevel = currentLevel < 4 ? ++currentLevel : 1; _gameCommands->LoadNewLevel(currentLevel); }));
		auto c = std::shared_ptr<Process>(new DelayProcess(5000));

		// Chain a set of subsequent processes
		a->AttachChild(b); b->AttachChild(c); c->AttachChild(d);

		processManager.AttachProcess(a);

		Logger::Get()->LogThis("All Pickups Collected Well Done!");
	}
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
	auto _player = networkPlayerJoinedEvent->_player;
	// Add the player to our level... find a suitable position for it
}

void LevelManager::GenerateNewLevel() { RemoveAllGameObjects(); CreateAutoLevel(); }

void LevelManager::RemoveAllGameObjects()
{
	auto& objects = SceneManager::Get()->GetGameWorld().GetGameObjects();
	std::for_each(std::begin(objects), std::end(objects), [this](std::shared_ptr<gamelib::GameObject> gameObject)
	{
		_eventManager->RaiseEvent(GameObjectEventFactory::MakeRemoveObjectEvent(&(*gameObject)), this);
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
		case 1: PlayLevelMusic("LevelMusic1"); break;
		case 2: PlayLevelMusic("LevelMusic2"); break;
		case 3: PlayLevelMusic("LevelMusic3"); break;
		case 4: PlayLevelMusic("LevelMusic4"); break;
		default:
		{
			std::stringstream message;
			message << "Unexpected level " << level;
			Logger::Get()->LogThis(message.str());
		} break;
	}
}

void LevelManager::PlayLevelMusic(std::string levelMusicAssetName)
{
	auto asset = ResourceManager::Get()->GetAssetInfo(levelMusicAssetName);
	if (asset->isLoadedInMemory)
	{
		if (asset && asset->isLoadedInMemory) { AudioManager::Get()->Play(gamelib::AudioManager::ToAudioAsset(asset)->AsMusic()); }
	}
}

void LevelManager::RemoveGameObject(gamelib::GameObject& gameObject)
{
	auto& gameObjects = SceneManager::Get()->GetGameWorld().GetGameObjects();
	
	// Look for gameObject
	auto findResult = std::find_if(begin(gameObjects), end(gameObjects), [&](weak_ptr<gamelib::GameObject> target)
	{ 
		if(auto underlyingGameObject = target.lock()) { return underlyingGameObject->Id == gameObject.Id; }
		return false;
	});
	
	auto found = findResult != end(gameObjects);
			
	if(found)
	{
		_eventManager->Unsubscribe((*findResult)->Id);
		gameObjects.erase(findResult);
	}

	GameData::Get()->SetGameObjects(&gameObjects);
}

void LevelManager::GetKeyboardInput()
{
	SDL_Event sdlEvent;
	auto keyState = SDL_GetKeyboardState(NULL);

	if (keyState[SDL_SCANCODE_UP] || keyState[SDL_SCANCODE_W]) { _gameCommands->MoveUp(_verbose); }
	if (keyState[SDL_SCANCODE_DOWN] || keyState[SDL_SCANCODE_S]) { _gameCommands->MoveDown(_verbose); }
	if (keyState[SDL_SCANCODE_LEFT] || keyState[SDL_SCANCODE_A]) { _gameCommands->MoveLeft(_verbose); }
	if (keyState[SDL_SCANCODE_RIGHT] || keyState[SDL_SCANCODE_D]) { _gameCommands->MoveRight(_verbose); }
		
	while (SDL_PollEvent(&sdlEvent))
	{		
		if (sdlEvent.type == SDL_KEYDOWN)
		{
			switch (sdlEvent.key.keysym.sym)
			{						
				case SDLK_q: 
				case SDLK_ESCAPE: _gameCommands->Quit(_verbose); break;
				case SDLK_h: _gameCommands->LoadNewLevel(1); currentLevel = 1; break;
				case SDLK_j: _gameCommands->LoadNewLevel(2); currentLevel = 2; break;
				case SDLK_k: _gameCommands->LoadNewLevel(3); currentLevel = 3; break;
				case SDLK_l: _gameCommands->LoadNewLevel(4); currentLevel = 4; break;
				case SDLK_1: _gameCommands->PlaySoundEffect(gamelib::AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo("high.wav"))->AsSoundEffect()); break;
				case SDLK_2: _gameCommands->PlaySoundEffect(gamelib::AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo("medium.wav"))->AsSoundEffect()); break;
				case SDLK_3: _gameCommands->PlaySoundEffect(gamelib::AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo("low.wav"))->AsSoundEffect()); break;
				case SDLK_4: _gameCommands->PlaySoundEffect(gamelib::AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo("scratch.wav"))->AsSoundEffect()); break;
				case SDLK_9: _gameCommands->ToggleMusic(_verbose); break;
				case SDLK_r: _gameCommands->ReloadSettings(_verbose); break;
				case SDLK_g: _gameCommands->GenerateNewLevel(_verbose); break;
				case SDLK_p: _gameCommands->PingGameServer(); break;
				case SDLK_n: _gameCommands->StartNetworkLevel(); break;
				case SDLK_SPACE: _gameCommands->Fire(_verbose); 	break;
			}
		}
		
		if (sdlEvent.type == SDL_QUIT)  {_gameCommands->Quit(_verbose); return; }
	}
}


shared_ptr<Player> LevelManager::CreatePlayer(const vector<shared_ptr<Room>> rooms, const int playerWidth, const int playerHeight) const
{		
	const auto playerRoomIndex = GetRandomIndex(0, rooms.size());	
	const auto& playerRoom = rooms[playerRoomIndex];
	const auto centerPositionInRoom = playerRoom->GetCenter(playerWidth, playerHeight);	
	const auto playerNickName = SceneManager::Get()->GetGameWorld().IsNetworkGame 
								? SettingsManager::Get()->GetString("networking", "nickname")
								: "Player1";

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

		auto pickup = std::shared_ptr<Pickup>(new Pickup(positionInRoom.GetX(), positionInRoom.GetY(), pickupWidth, pickupHeight, true, rand_index));
		
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
	auto& rooms = level->Rooms;

	InitializeRooms(rooms, gameObjectsPtr);	

	// Create the pickups
	const auto pickupWidth = rowWidth / 2;
	const auto pickupHeight = rowHeight / 2;
	const auto pickups = CreatePickups(rooms, pickupWidth, pickupHeight);

	// Create the player
	auto playerSpriteAsset = dynamic_pointer_cast<SpriteAsset>(ResourceManager::Get()->GetAssetInfo("edge_player"));
	const auto _player = CreatePlayer(rooms, playerSpriteAsset->Dimensions.GetWidth(), playerSpriteAsset->Dimensions.GetHeight());
	
	InitializePlayer(_player, playerSpriteAsset);
	RegisterGameObject(_player);

	// Create Hud
	_hudItem = StaticSprite::Create(rooms[rooms.size() - 1]->GetCenter(_player->GetWidth(), _player->GetHeight()), 
		dynamic_pointer_cast<gamelib::SpriteAsset>(ResourceManager::Get()->GetAssetInfo("hudspritesheet")));

	InitializeHudItem(_hudItem);
	RegisterGameObject(_hudItem);

	// Setup the pickups
	InitializePickups(pickups, gameObjectsPtr);
	
	// Add single UI elements to the scene
	AddGameObjectToScene(_player);
	AddGameObjectToScene(_hudItem);

	// Add the framerate
	drawableFrameRate = std::shared_ptr<DrawableFrameRate>(new DrawableFrameRate(&level->Rooms[level->Rooms.size()-2]->Bounds));
	AddGameObjectToScene(drawableFrameRate);
	RegisterGameObject(drawableFrameRate);

	GameData::Get()->SetGameObjects(&gameObjectsPtr);	

	return gameObjectsPtr;
}

void LevelManager::InitializeHudItem(std::shared_ptr<StaticSprite> _hudItem)
{
	_hudItem->LoadSettings();
	_hudItem->SubscribeToEvent(EventType::PlayerMovedEventType);
}

void LevelManager::AddGameObjectToScene(std::shared_ptr<gamelib::GameObject> object) { _eventManager->RaiseEvent(std::dynamic_pointer_cast<gamelib::Event>(_eventFactory->CreateAddToSceneEvent(object)), this); }

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
	const auto _player = CreatePlayer(rooms, playerSpriteAsset->Dimensions.GetWidth(), playerSpriteAsset->Dimensions.GetHeight());

	// Setup the player
	InitializePlayer(_player, playerSpriteAsset);

	// Create the pickups
	const auto pickupWidth = 32;
	const auto pickupHeight = 32;
	const auto pickups = CreatePickups(rooms, pickupWidth, pickupHeight);

	// Setup the pickups
	InitializePickups(pickups, gameObjectsPtr);
	
	// Add player to game world
	RegisterGameObject(_player);

	GameData::Get()->SetGameObjects(&gameObjectsPtr);
		
	return gameObjectsPtr;
}

void LevelManager::InitializePlayer(std::shared_ptr<Player> _player, std::shared_ptr<gamelib::SpriteAsset> spriteAsset)
{
	_player->SetMoveStrategy(shared_ptr<PlayerMoveStrategy>(new PlayerMoveStrategy(_player, 2)));
	_player->SetTag(constants::playerTag);
	_player->LoadSettings();
	_player->SetSprite(AnimatedSprite::Create(_player->Position.GetX(), _player->Position.GetY(), spriteAsset));

	// We keep a reference to track of the player globally
	SceneManager::Get()->GetGameWorld()._player = _player;
}

void LevelManager::InitializePickups(const ListOfGameObjects& pickups, ListOfGameObjects& gameObjectsPtr)
{
	for (const auto& pickup : pickups)
	{
		pickup->LoadSettings();
		pickup->SubscribeToEvent(EventType::PlayerMovedEventType);

		AddGameObjectToScene(pickup);
		RegisterGameObject(pickup);
	}
}

void LevelManager::InitializeRooms(std::vector<std::shared_ptr<Room>>& rooms, ListOfGameObjects& gameObjects)
{
	for (const auto& room : rooms)
	{
		room->LoadSettings();
		room->SubscribeToEvent(EventType::PlayerMovedEventType);
		room->SubscribeToEvent(EventType::SettingsReloaded);

		AddGameObjectToScene(room);
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
	catch (...) { LogMessage("Could not start level for unknown reasons. level=" + std::to_string(level)); 	}
	return isSuccess;
}

void LevelManager::RegisterGameObject(std::shared_ptr<GameObject> obj) { SceneManager::Get()->GetGameWorld().GetGameObjects().push_back(obj); }
int LevelManager::ReducePickupCount() { return --numLevelPickups; }
int LevelManager::IncreasePickupCount() { return ++numLevelPickups; }
shared_ptr<Level> LevelManager::GetLevel() { return level; }

LevelManager* LevelManager::Get()
{
	if (Instance == nullptr) { Instance = new LevelManager(); }
	return Instance;
}

LevelManager::~LevelManager() { Instance = nullptr; }
LevelManager* LevelManager::Instance = nullptr;