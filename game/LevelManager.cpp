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
#include "EdgeTowardsRoomStrategy.h"
#include "GameObjectEventFactory.h"
#include <GameData.h>
#include <sstream>
#include <events/NetworkPlayerJoinedEvent.h>
#include <events/StartNetworkLevelEvent.h>
#include <Level.h>
#include <Rooms.h>
#include <objects/StaticSprite.h>
#include <events/DoLogicUpdateEvent.h>
#include <events/SceneChangedEvent.h>

using namespace gamelib;
using namespace std;

bool LevelManager::Initialize()
{
	InitGameWorldData();

	eventManager = EventManager::Get();
	eventFactory = EventFactory::Get();

	_gameCommands = shared_ptr<GameCommands>(new GameCommands());

	eventManager->SubscribeToEvent(EventType::GenerateNewLevel, this);
	eventManager->SubscribeToEvent(EventType::InvalidMove, this);
	eventManager->SubscribeToEvent(EventType::FetchedPickup, this);
	eventManager->SubscribeToEvent(EventType::GameObject, this);
	eventManager->SubscribeToEvent(EventType::LevelChangedEventType, this);
	eventManager->SubscribeToEvent(EventType::NetworkPlayerJoined, this);
	eventManager->SubscribeToEvent(EventType::StartNetworkLevel, this);

	verbose = SettingsManager::Get()->GetBool("global", "verbose");

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
	case EventType::InvalidMove:
		_gameCommands->InvalidMove();
		break;
	case EventType::FetchedPickup:
		_gameCommands->FetchedPickup();
		hudItem->AdvanceFrame();
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
	auto objects = SceneManager::Get()->GetGameWorld().GetGameObjects();
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
		auto audioAsset = gamelib::AudioManager::ToAudioAsset(asset);
		if (asset && asset->isLoadedInMemory)
		{
			AudioManager::Get()->Play(audioAsset->AsMusic());
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

void LevelManager::InitGameWorldData() const
{	
	SceneManager::Get()->GetGameWorld().IsGameDone = false;
	SceneManager::Get()->GetGameWorld().IsNetworkGame = false;
	SceneManager::Get()->GetGameWorld().CanDraw = true;
}

void LevelManager::GetKeyboardInput()
{
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent))
	{
		if (sdlEvent.type == SDL_KEYDOWN)
		{
			switch (sdlEvent.key.keysym.sym)
			{
			case SDLK_SPACE:				
				_gameCommands->Fire(verbose);
				break;
			case SDLK_w:
			case SDLK_UP:				
				_gameCommands->MoveUp(verbose);
				break;
			case SDLK_s:
			case SDLK_DOWN:				
				_gameCommands->MoveDown(verbose);
				break;
			case SDLK_a:
			case SDLK_LEFT:				
				_gameCommands->MoveLeft(verbose);
				break;
			case SDLK_d:
			case SDLK_RIGHT:				
				_gameCommands->MoveRight(verbose);
				break;
			case SDLK_q:
			case SDLK_ESCAPE:
				_gameCommands->Quit(verbose);
				break;
			case SDLK_h:	
				_gameCommands->LoadNewLevel(1);
				break;
			case SDLK_j:
				_gameCommands->LoadNewLevel(2);
				break;
			case SDLK_k:
				_gameCommands->LoadNewLevel(3);
				break;
			case SDLK_l:
				_gameCommands->LoadNewLevel(4);
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
			default:
				std::cout << "Unknown control key" << std::endl;
				LogMessage("Unknown control key", verbose);
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

shared_ptr<gamelib::GameObject> LevelManager::CreatePlayer(const vector<shared_ptr<Room>> rooms, const int playerWidth, const int playerHeight) const
{	
	const auto minNumRooms = 0;
	const auto playerRoomIndex = GetRandomIndex(minNumRooms, rooms.size());
	const auto playerRoom = rooms[playerRoomIndex];
	const auto positionInRoom = playerRoom->GetCenter(playerWidth, playerHeight);	
	const auto playerNickName = SceneManager::Get()->GetGameWorld().IsNetworkGame ? 
								SettingsManager::Get()->GetString("networking", "nickname")
								: "Player1";
	const auto player =  shared_ptr<Player>(new Player(positionInRoom.GetX(), positionInRoom.GetY(), playerWidth, playerHeight, playerNickName));	
	auto moveStrategy = SettingsManager::Get()->GetString("player", "moveStrategy");

	std::string spriteAssetName;

	GetPlayerAssetName(moveStrategy, player, spriteAssetName);

	player->SetPlayerRoom(playerRoomIndex);
	player->SetTag(constants::playerTag);
	player->LoadSettings();
	player->RaiseEvent(std::make_shared<AddGameObjectToCurrentSceneEvent>(player, 100));
	player->CenterPlayerInRoom(playerRoom);

	auto spriteAsset = dynamic_pointer_cast<SpriteAsset>(ResourceManager::Get()->GetAssetInfo(spriteAssetName));
	auto sprite = AnimatedSprite::Create(player->Position.GetX(), player->Position.GetY(), spriteAsset);
		
	player->SetSprite(sprite);

	// We keep track of the player globally
	SceneManager::Get()->GetGameWorld().player = player;

	return player;
}

void LevelManager::GetPlayerAssetName(std::string& moveStrategy, const std::shared_ptr<Player>& player, std::string& spriteAssetName) const
{	
	player->SetMoveStrategy(shared_ptr<EdgeTowardsRoomStrategy>(new EdgeTowardsRoomStrategy(player, 2)));
	spriteAssetName = "edge_player";
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
	level = std::shared_ptr<Level>(new Level(filename));	
	level->Load();
		
	const auto rowWidth = SettingsManager::Get()->GetInt("global", "screen_width") / level->NumCols;
	const auto rowHeight = SettingsManager::Get()->GetInt("global", "screen_height") / level->NumRows;

	auto& gameObjectsPtr = SceneManager::Get()->GetGameWorld().GetGameObjects();
	
	auto rooms = level->Rooms;

	InitializeRooms(rooms, gameObjectsPtr);	

	// Create the pickups
	const auto pickupWidth = rowWidth / 2;
	const auto pickupHeight = rowHeight / 2;
	const auto pickups = CreatePickups(rooms, pickupWidth, pickupHeight);

	// Create the player
	const auto playerWidth = rowWidth / 2;
	const auto playerHeight = rowHeight / 2;
	const auto player = CreatePlayer(rooms, playerWidth, playerHeight);

	// Create Hud

	auto spriteAsset = dynamic_pointer_cast<gamelib::SpriteAsset>(ResourceManager::Get()->GetAssetInfo("hudspritesheet"));

	auto center = Rooms::CenterOfRoom(rooms[rooms.size() - 1], 32, 32);
	hudItem = StaticSprite::Create(center.GetX(), center.GetY(), spriteAsset);
	hudItem->LoadSettings();
	hudItem->Initialize();
	hudItem->SubscribeToEvent(EventType::PlayerMovedEventType);
	hudItem->SubscribeToEvent(EventType::DoLogicUpdateEventType);

	auto gameObject = std::dynamic_pointer_cast<GameObject>(hudItem);
	hudItem->RaiseEvent(std::shared_ptr<AddGameObjectToCurrentSceneEvent>(new AddGameObjectToCurrentSceneEvent(gameObject)));

	gameObjectsPtr.push_back(hudItem);

	// Setup the pickups
	InitializePickups(pickups, gameObjectsPtr);

	// Add player to game world
	gameObjectsPtr.push_back(player);

	GameData::Get()->SetGameObjects(&gameObjectsPtr);

	return gameObjectsPtr;
}



ListOfGameObjects LevelManager::CreateAutoLevel()
{
	ResourceManager::Get()->IndexResourceFile();
	LevelManager::Get()->ChangeLevel(1);

	const auto rows = SettingsManager::Get()->GetInt("grid", "rows");
	const auto cols = SettingsManager::Get()->GetInt("grid", "cols");
	const auto screenWidth = SettingsManager::Get()->GetInt("global", "screen_width");
	const auto screenHeight = SettingsManager::Get()->GetInt("global", "screen_height");
	const auto removeRandomSides = SettingsManager::Get()->GetBool("grid", "removeSidesRandomly");
	const auto rowWidth = screenWidth / cols; 
	const auto rowHeight = screenHeight / rows;

	auto& gameObjectsPtr = SceneManager::Get()->GetGameWorld().GetGameObjects();
	
	// Generate the level's rooms
	
	auto roomGenerator = RoomGenerator(screenWidth, screenHeight, rows, cols, removeRandomSides);
	auto rooms = roomGenerator.Generate();
	
	InitializeRooms(rooms, gameObjectsPtr);
			
	// Create the player
	const auto playerWidth = rowWidth/2;
	const auto playerHeight = rowHeight/2;
	const auto player = CreatePlayer(rooms, playerWidth, playerHeight);

	// Create the pickups
	const auto pickupWidth = rowWidth/2;
	const auto pickupHeight = rowHeight/2;
	const auto pickups = CreatePickups(rooms, pickupWidth, pickupHeight);

	// Setup the pickups
	InitializePickups(pickups, gameObjectsPtr);
	
	// Add player to game world
	gameObjectsPtr.push_back(player);	

	GameData::Get()->SetGameObjects(&gameObjectsPtr);
		
	return gameObjectsPtr;
}

void LevelManager::InitializePickups(const ListOfGameObjects& pickups, ListOfGameObjects& gameObjectsPtr)
{
	for (const auto& pickup : pickups)
	{
		pickup->LoadSettings();
		pickup->SubscribeToEvent(EventType::PlayerMovedEventType);
		pickup->SubscribeToEvent(EventType::DoLogicUpdateEventType);

		auto gameObject = std::dynamic_pointer_cast<Pickup>(pickup);
		pickup->RaiseEvent(std::shared_ptr<AddGameObjectToCurrentSceneEvent>(new AddGameObjectToCurrentSceneEvent(gameObject)));

		gameObjectsPtr.push_back(pickup);
	}
}

void LevelManager::InitializeRooms(std::vector<std::shared_ptr<Room>>& rooms, ListOfGameObjects& gameObjectsPtr)
{
	for (const auto& room : rooms)
	{
		room->LoadSettings();
		room->SubscribeToEvent(EventType::PlayerMovedEventType);
		room->SubscribeToEvent(EventType::SettingsReloaded);

		room->RaiseEvent(std::make_shared<AddGameObjectToCurrentSceneEvent>(std::dynamic_pointer_cast<Room>(room)));

		gameObjectsPtr.push_back(dynamic_pointer_cast<gamelib::GameObject>(room));
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
