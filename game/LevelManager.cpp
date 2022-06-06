#include "LevelManager.h"
#include "common/Common.h"
#include "Pickup.h"
#include "common/constants.h"
#include "events/AddGameObjectToCurrentSceneEvent.h"
#include "LevelGenerator.h"
#include "objects/game_world_component.h"
#include <events/GameObjectEvent.h>
#include "Player.h"
#include <memory>
#include <events/SceneChangedEvent.h>
#include <SpriteAsset.h>
#include <objects/GameObjectFactory.h>
#include "SnapToRoomStrategy.h"
#include "EdgeTowardsRoomStrategy.h"
#include "GameObjectEventFactory.h"

using namespace gamelib;
using namespace std;

/// <summary>
/// Initialize the LevelManager. 
/// - Subscribe to level type events
/// </summary>
/// <returns>true if initializd, false otherwise</returns>
bool LevelManager::Initialize()
{
	// Set basic game world defaults
	InitGameWorldData();

	// All game commands that we handle in this game.
	_gameCommands = shared_ptr<GameCommands>(new GameCommands());

	// Subscribe to game events
	EventManager::Get()->SubscribeToEvent(EventType::GenerateNewLevel, this);
	EventManager::Get()->SubscribeToEvent(EventType::InvalidMove, this);
	EventManager::Get()->SubscribeToEvent(EventType::FetchedPickup, this);
	EventManager::Get()->SubscribeToEvent(EventType::GameObject, this);
	EventManager::Get()->SubscribeToEvent(EventType::LevelChangedEventType, this);
		
	return true;
}

/// <summary>
/// Prepares the scene for the level
/// </summary>
/// <param name="level"></param>
/// <returns></returns>
bool LevelManager::ChangeLevel(int level)
{
	bool isSuccess = false;
	try
	{
		_gameCommands->ChangeLevel(false, level);
		isSuccess = true;
	}
	catch (...)
	{
		LogMessage("Could not start level for unknown reasons. level=" + std::to_string(level));
	}
	return isSuccess;
}

/// <summary>
/// Handle level events
/// </summary>
/// <param name="evt">Incoming level event</param>
/// <returns>Any events generated as a result of handling incoming level event</returns>
events LevelManager::HandleEvent(std::shared_ptr<Event> evt)
{
	gamelib::events newEvents;
	std::shared_ptr<GameObjectEvent> gameObjectEvent = nullptr;
	
	switch(evt->type)
	{
	case EventType::InvalidMove:
		_gameCommands->InvalidMove();
		break;
	case EventType::FetchedPickup:
		_gameCommands->FetchedPickup();
		break;
	case EventType::GameObject:
		gameObjectEvent = dynamic_pointer_cast<GameObjectEvent>(evt);
		
		// Deal with GameObject contextual events:
		switch(gameObjectEvent->context)
		{
			case GameObjectEventContext::Remove:
				RemoveGameObject(*gameObjectEvent->gameObject);
			break;
		}
		break;
	case EventType::GenerateNewLevel:
		GenerateNewLevel();
		break;
	case EventType::LevelChangedEventType:
		OnLevelChanged(evt);
		break;
	}

	return newEvents;
}

void LevelManager::GenerateNewLevel()
{
	RemoveAllGameObjects();
	CreateAutoLevel();
}

/// <summary>
/// Raises events to add game world objects to the game
/// </summary>
void LevelManager::RemoveAllGameObjects()
{
	std::for_each(std::begin(SceneManager::Get()->GetGameWorld().GetGameObjects()), std::end(SceneManager::Get()->GetGameWorld().GetGameObjects()), [this](std::shared_ptr<GameObject> gameObject) {
		EventManager::Get()->RaiseEvent(GameObjectEventFactory::MakeRemoveObjectEvent(&(*gameObject)), this);
	});
}


void LevelManager::OnLevelChanged(std::shared_ptr<gamelib::Event>& evt)
{
	const auto levelChangedEvent = dynamic_pointer_cast<SceneChangedEvent>(evt);
	auto level = levelChangedEvent->scene_id;

	// Could encode this into the scene file instead of hardcoding it
	if (level == 1)
	{
		PlayLevelMusic("LevelMusic1");
	}
	if (level == 2)
	{
		PlayLevelMusic("LevelMusic2");
	}
	if (level == 3)
	{
		PlayLevelMusic("LevelMusic3");
	}
	if (level == 4)
	{
		PlayLevelMusic("LevelMusic4");
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

/// <summary>
/// Provide event subsystem with our name
/// </summary>
std::string LevelManager::GetSubscriberName() 
{ 
	return "level_manager";
};

/// <summary>
/// Remove Game Object from Game world
/// </summary>
/// <param name="gameWorld">Game world container</param>
/// <param name="gameObject">game Object</param>
void LevelManager::RemoveGameObject(GameObject& gameObject)
{
	auto& objects = SceneManager::Get()->GetGameWorld().GetGameObjects();
	
	// Look for gameObject
	auto result = std::find_if(begin(objects), end(objects), [&](weak_ptr<gamelib::GameObject> target)
	{ 
		if(auto underlyingGameObject = target.lock()) // Test to see if the game object reference still exists 
		{
			auto result = (underlyingGameObject->Id == gameObject.Id);
			return result;
		}
		return false;
	});
	
	auto found = result != end(objects);
			
	if(found)
	{			
		// Remove any pending events in the event manager for this object
		EventManager::Get()->Unsubscribe((*result)->Id);

		// Erase from list of known game object
		objects.erase(result);
	}
}

/// <summary>
/// Initialize game world
/// </summary>
void LevelManager::InitGameWorldData() const
{	
	SceneManager::Get()->GetGameWorld().IsGameDone = false;
	SceneManager::Get()->GetGameWorld().IsNetworkGame = false;
	SceneManager::Get()->GetGameWorld().CanDraw = true;
}

/// <summary>
/// Get controller input
/// </summary>
void LevelManager::GetKeyboardInput()
{
	SDL_Event sdlEvent;
	const auto bVerbose = SettingsManager::Get()->GetBool("global", "verbose");
	
	while (SDL_PollEvent(&sdlEvent) != 0)
	{
		// look for any key press
		if (sdlEvent.type == SDL_KEYDOWN)
		{
			switch (sdlEvent.key.keysym.sym)
			{
			case SDLK_SPACE:				
				_gameCommands->Fire(bVerbose);
				break;
			case SDLK_w:
			case SDLK_UP:				
				_gameCommands->MoveUp(bVerbose);
				break;
			case SDLK_s:
			case SDLK_DOWN:				
				_gameCommands->MoveDown(bVerbose);
				break;
			case SDLK_a:
			case SDLK_LEFT:				
				_gameCommands->MoveLeft(bVerbose);
				break;
			case SDLK_d:
			case SDLK_RIGHT:				
				_gameCommands->MoveRight(bVerbose);
				break;
			case SDLK_q:
			case SDLK_ESCAPE:
				_gameCommands->Quit(bVerbose);
				break;
			case SDLK_j:	
				GenerateNewLevel();
				_gameCommands->ChangeLevel(bVerbose, 1);
				break;
			case SDLK_k:
				GenerateNewLevel();
				_gameCommands->ChangeLevel(bVerbose, 2);
				break;
			case SDLK_l:
				GenerateNewLevel();
				_gameCommands->ChangeLevel(bVerbose, 3);
				break;
			case SDLK_x:
				GenerateNewLevel();
				_gameCommands->ChangeLevel(bVerbose, 4);
				break;
			case SDLK_1:
				AudioManager::Get()->Play(gamelib::AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo("high.wav"))->AsSoundEffect());
				break;
			case SDLK_2:
				AudioManager::Get()->Play(gamelib::AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo("medium.wav"))->AsSoundEffect());
				break;
			case SDLK_3:
				AudioManager::Get()->Play(gamelib::AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo("low.wav"))->AsSoundEffect());
				break;
			case SDLK_4:
				AudioManager::Get()->Play(gamelib::AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo("scratch.wav"))->AsSoundEffect());
				break;
			case SDLK_9:
				_gameCommands->ToggleMusic(bVerbose);
				break;
			case SDLK_r:				
				_gameCommands->ReloadSettings(bVerbose);
				break;
			case SDLK_g:				
				_gameCommands->GenerateNewLevel(bVerbose);
				break;
			default:
				std::cout << "Unknown control key" << std::endl;
				LogMessage("Unknown control key", bVerbose);
				break;
			}
		}
		
		// quit
		if (sdlEvent.type == SDL_QUIT)
		{
			_gameCommands->Quit(bVerbose);
			return;
		}
	}
}

/// <summary>
/// Get random number between [min, max]
/// </summary>
size_t LevelManager::GetRandomIndex(const int min, const int max) { return rand() % (max - min + 1) + min; }

/// <summary>
/// Gets the coordinates of the center of a room
/// </summary>
/// <param name="room">room</param>
/// <param name="w">with of the room</param>
/// <param name="h">height of the room</param>
/// <returns>coordinate of the center of the room</returns>
coordinate<int> GetCenterOfRoom(const Room &room, const int w, const int h)
{
	auto const room_x_mid = room.GetX() + (room.GetWidth() / 2);
	auto const room_y_mid = room.GetY() + (room.GetHeight() / 2);
	auto const x = room_x_mid - w /2;
	auto const y = room_y_mid - h /2;			
	return coordinate<int>(x, y);
}

/// <summary>
/// Create the player
/// </summary>
shared_ptr<GameObject> LevelManager::CreatePlayer(const vector<shared_ptr<Room>> rooms, const int w, const int h) const
{	
	const auto minNumRooms = 0;
	const auto playerRoomIndex = GetRandomIndex(minNumRooms, rooms.size());
	const auto playerRoom = rooms[playerRoomIndex];
	const auto positionInRoom = GetCenterOfRoom(*playerRoom, w, h);
	
	// create the player
	const auto player =  shared_ptr<Player>(new Player(positionInRoom.GetX(), positionInRoom.GetY(), w, h));
	
	auto moveStrategy = SettingsManager::Get()->GetString("player", "moveStrategy");
	std::string spriteAssetName;
	if(moveStrategy == "snap")
	{
		player->SetMoveStrategy(shared_ptr<SnapToRoomStrategy>(new SnapToRoomStrategy(player)));
		spriteAssetName = "snap_player";
	}
	else if(moveStrategy == "edge")
	{
		player->SetMoveStrategy(shared_ptr<EdgeTowardsRoomStrategy>(new EdgeTowardsRoomStrategy(player, 2)));
		spriteAssetName = "edge_player";
	}
	else
	{
		THROW(12, "Unknown Move strategy", "Create Player");
	}

	player->SetRoom(playerRoomIndex);	

	player->SetTag(constants::playerTag);
	
	// Load in external settings for the player
	player->LoadSettings();

	// Add the player to the current scene/level
	player->RaiseEvent(std::make_shared<AddGameObjectToCurrentSceneEvent>(player, 100));

	// Place the player in a random room
	player->CenterPlayerInRoom(playerRoom);

	// Setup the player sprite
	
	
	auto spriteAsset = dynamic_pointer_cast<SpriteAsset>(ResourceManager::Get()->GetAssetInfo(spriteAssetName));
	auto sprite = AnimatedSprite::Create(player->x, player->y, spriteAsset);

	// Set player's sprite
	player->SetSprite(sprite);
	SceneManager::Get()->GetGameWorld().player = player;

	player->animationTimeoutTimer.Start(150);

	return player;
}

/// <summary>
/// Create the pickups
/// </summary>
ListOfGameObjects LevelManager::CreatePickups(const vector<shared_ptr<Room>>& rooms, const int w, const int h)
{
	ListOfGameObjects pickups;
	const auto num_pickups = 10;
	auto part = num_pickups / 3;

	for(auto i = 0; i < num_pickups; i++)
	{
		const auto rand_index = GetRandomIndex(0, rooms.size()-1);
		const auto random_room = rooms[rand_index];		

		// Place the pickup in the center of the candidate room
		const auto positionInRoom = GetCenterOfRoom(*random_room, w, h);
		auto npc = std::shared_ptr<Pickup>(new Pickup(positionInRoom.GetX(), positionInRoom.GetY(), w, h, true));

		// Place 3 sets of types of pickups		
		if(i < 1 * part) 
		{
			npc->stringProperties["assetName"] = SettingsManager::Get()->GetString("pickup1", "assetName");		
		}
		else if(i >= 1 * part && i < 2 * part) 
		{ 
			npc->stringProperties["assetName"] = SettingsManager::Get()->GetString("pickup2", "assetName"); 
		}
		else if(i >= 2 * part)
		{
			npc->stringProperties["assetName"] = SettingsManager::Get()->GetString("pickup3", "assetName");
		}

		npc->Initialize();

		pickups.push_back(npc);
	}
	return pickups;
}

/// <summary>
/// Create the level objects
/// </summary>
/// <returns>List of level game objects</returns>
ListOfGameObjects LevelManager::CreateAutoLevel()
{
	// Read level settings
	const auto rows = SettingsManager::Get()->GetInt("grid", "rows");
	const auto cols = SettingsManager::Get()->GetInt("grid", "cols");
	const auto screenWidth = SettingsManager::Get()->GetInt("global", "screen_width");
	const auto screenHeight = SettingsManager::Get()->GetInt("global", "screen_height");
	const auto rowWidth = screenWidth / cols; 
	const auto rowHeight = screenHeight / rows;

	// Index Resources File
	ResourceManager::Get()->IndexResources();
	
	// This is the list of all game objects
	ListOfGameObjects& gameObjectsPtr = SceneManager::Get()->GetGameWorld().GetGameObjects();
	
	// Generate the level's rooms
	
	const auto removeRandomSides = SettingsManager::Get()->GetBool("grid", "removeSidesRandomly");
	auto levelGenerator = LevelGenerator(screenWidth, screenHeight, rows, cols, removeRandomSides);
	auto rooms = levelGenerator.Generate();

	// Setup each room
	for (const auto& room: rooms)
	{		
		// Initialize each room's initial settings
		room->LoadSettings();

		// room's will want to know when the player moved for collision detection etc
		room->SubscribeToEvent(EventType::PlayerMovedEventType);
		
		// Add each room to the scene
		room->RaiseEvent(std::make_shared<AddGameObjectToCurrentSceneEvent>(std::dynamic_pointer_cast<Room>(room)));

		// Allow for room's settings to be modified while playing
		room->SubscribeToEvent(EventType::SettingsReloaded);

		// Add each room as a game object
		gameObjectsPtr.push_back(dynamic_pointer_cast<GameObject>(room));
	}
		
	// Create the player
	const auto player = CreatePlayer(rooms, rowWidth/2, rowHeight/2);

	// Create the pickups
	const auto pickups = CreatePickups(rooms, rowWidth/2, rowHeight/2);

	// Setup the pickups
	for(const auto &pickup : pickups)
	{
		auto gameObject = std::dynamic_pointer_cast<Pickup>(pickup);
		auto addEvent = new AddGameObjectToCurrentSceneEvent(gameObject);

		// Add to current scene
		pickup->RaiseEvent(std::shared_ptr<AddGameObjectToCurrentSceneEvent>(addEvent));
		
		// Subscribe to player movement notifications
		pickup->SubscribeToEvent(EventType::PlayerMovedEventType);

		// Subscribe to update events
		pickup->SubscribeToEvent(gamelib::EventType::DoLogicUpdateEventType);
		
		gameObjectsPtr.push_back(pickup);		
	}
	
	// Add player to game world
	gameObjectsPtr.push_back(player);	
		
	return gameObjectsPtr;
}
