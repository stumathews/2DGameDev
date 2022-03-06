#include "LevelManager.h"
#include "common/Common.h"
#include "Pickup.h"
#include "common/constants.h"
#include "events/AddGameObjectToCurrentSceneEvent.h"
#include "game/LevelGenerator.h"
#include "objects/game_world_component.h"
#include <events/GameObjectEvent.h>
#include <objects/Player.h>
#include <memory>

using namespace gamelib;
using namespace std;

LevelManager::LevelManager(	
	EventManager& _eventManager, 	
	ResourceManager& _resourceManager, 
	SettingsManager& _settingsManager, 
	GameWorld& gameWorld,
	SceneManager& scene_admin, 
	AudioManager& _audioManager, 
	Logger& _gameLogger) 
		: _eventManager(_eventManager), _resourceManager(_resourceManager), _settingsManager(_settingsManager), gameWorld(gameWorld), _sceneManager(scene_admin), _audioManager(_audioManager), _gameLogger(_gameLogger) 
{ }

/// <summary>
/// Initialize the LevelManager
/// </summary>
/// <returns>true if initializd, false otherwise</returns>
bool LevelManager::Initialize()
{
	// set basic game world defaults
	InitGameWorldData();

	// All game commands that we handle in this game
	_gameCommands = shared_ptr<GameCommands>(new GameCommands(_settingsManager, _eventManager, _audioManager, _resourceManager, gameWorld, _gameLogger));

	// subscribe to game events
	_eventManager.SubscribeToEvent(EventType::GenerateNewLevel, this);
	_eventManager.SubscribeToEvent(EventType::InvalidMove, this);
	_eventManager.SubscribeToEvent(EventType::FetchedPickup, this);
	_eventManager.SubscribeToEvent(EventType::GameObject, this);
		
	return true;
}

/// <summary>
/// Prepares the scene for the level
/// </summary>
/// <param name="level"></param>
/// <returns></returns>
bool LevelManager::PrepareLevel(int level)
{
	bool isSuccess = false;
	try
	{		
		// Trigger scene change 
		_gameCommands->ChangeLevel(false, level);
		isSuccess = true;
	}
	catch (...)
	{
		LogMessage("Could not start level for unknown reasons. level=" + std::to_string(level), _gameLogger);
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
	gamelib::events secondary_events;
	
	// Invalid move 
	if(evt->type == EventType::InvalidMove)
		_gameCommands->InvalidMove();		
	
	// Recieved pickup
	if(evt->type == EventType::FetchedPickup)
		_gameCommands->FetchedPickup();
	
	// Received per GameObject event
	if(evt->type == EventType::GameObject)
	{		
		const auto gameObjectEvent = dynamic_pointer_cast<GameObjectEvent>(evt);
		
		// Level Manager is responsible for removing game object from game
		if(gameObjectEvent->context == GameObjectEventContext::Remove)
		{
			RemoveGameObject(gameWorld, *gameObjectEvent->gameObject );			
		}

	}
	return secondary_events;
}

/// <summary>
/// Provide event subsystem with our name
/// </summary>
/// <returns></returns>
std::string LevelManager::GetSubscriberName() 
{ 
	return "level_manager";
};

/// <summary>
/// Remove Game Object from Game world
/// </summary>
/// <param name="gameWorld">Game world container</param>
/// <param name="gameObject">game Object</param>
void LevelManager::RemoveGameObject(GameWorld& gameWorld, GameObject& gameObject)
{
	auto &objects = gameWorld.objects;

	// Look for gameObject
	auto result = std::find_if(begin(objects), end(objects), [&](weak_ptr<gamelib::GameObject> target)
	{ 
		if(auto underlyingGameObject = target.lock()) // Test to see if the game object reference still exists 
		{
			auto result = (underlyingGameObject->id == gameObject.id);
			return result;
		}
		return false;
	});
	
	auto found = result != end(objects);
			
			if(found)
			{			
				// Remove any pending events in the event manager for this object
				_eventManager.Unsubscribe((*result)->id);

				// Erase from list of known game object
				objects.erase(result);
			}
}

/// <summary>
/// Initialize game world
/// </summary>
void LevelManager::InitGameWorldData() const
{
	gameWorld.IsGameDone = false;
	gameWorld.IsNetworkGame = false;
	gameWorld.CanDraw = true;
}

/// <summary>
/// Get controller input
/// </summary>
void LevelManager::GetKeyboardInput()
{
	SDL_Event sdlEvent;
	const auto bVerbose = _settingsManager.get_bool("global", "verbose");
	
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
				_gameCommands->ChangeLevel(bVerbose, 1);
				break;
			case SDLK_k:
				_gameCommands->ChangeLevel(bVerbose, 2);
				break;
			case SDLK_l:
				_gameCommands->ChangeLevel(bVerbose, 3);
				break;
			case SDLK_x:
				_gameCommands->ChangeLevel(bVerbose, 4);
				break;
			case SDLK_1:
				_audioManager.PlaySound(gamelib::AudioManager::ToAudioAsset(_resourceManager.GetAssetInfo("high.wav"))->AsSoundEffect());
				break;
			case SDLK_2:
				_audioManager.PlaySound(gamelib::AudioManager::ToAudioAsset(_resourceManager.GetAssetInfo("medium.wav"))->AsSoundEffect());
				break;
			case SDLK_3:
				_audioManager.PlaySound(gamelib::AudioManager::ToAudioAsset(_resourceManager.GetAssetInfo("low.wav"))->AsSoundEffect());
				break;
			case SDLK_4:
				_audioManager.PlaySound(gamelib::AudioManager::ToAudioAsset(_resourceManager.GetAssetInfo("scratch.wav"))->AsSoundEffect());
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
				LogMessage("Unknown control key", _gameLogger, bVerbose);
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
/// <param name="min"></param>
/// <param name="max"></param>
/// <returns></returns>
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
/// <param name="rooms"></param>
/// <param name="w"></param>
/// <param name="h"></param>
/// <returns></returns>
shared_ptr<GameObject> LevelManager::CreatePlayer(const vector<shared_ptr<Room>> rooms, const int w, const int h) const
{	
	const auto minNumRooms = 0;
	const auto playerRoomIndex = GetRandomIndex(minNumRooms, rooms.size());
	const auto playerRoom = rooms[playerRoomIndex];
	const auto positionInRoom = GetCenterOfRoom(*playerRoom, w, h);
	
	// create the player
	const auto player =  shared_ptr<Player>(new Player(positionInRoom.GetX(), positionInRoom.GetY(), w, h, _settingsManager, _eventManager, _gameLogger));
	player->SetRoom(playerRoomIndex);

	player->SetTag(constants::playerTag);
	
	// Load in external settings for the player
	player->LoadSettings(_settingsManager);

	// Player will respond to directional keyboard input
	player->SubscribeToEvent(EventType::PositionChangeEventType, _eventManager);

	// Player will be able to have its settings re-loaded at runtime
	player->SubscribeToEvent(EventType::SettingsReloaded, _eventManager);

	// Player can update itself 
	player->SubscribeToEvent(EventType::DoLogicUpdateEventType, _eventManager);

	// Add the player to the current scene/level
	player->RaiseEvent(std::make_shared<AddGameObjectToCurrentSceneEvent>(player, 100), _eventManager);

	// Place the player in a random room
	player->CenterPlayerInRoom(playerRoom);

	return player;
}

/// <summary>
/// Create the pickups
/// </summary>
/// <param name="rooms"></param>
/// <param name="w"></param>
/// <param name="h"></param>
/// <returns></returns>
ListOfGameObjects LevelManager::CreatePickups(const vector<shared_ptr<Room>>& rooms, const int w, const int h)
{
	ListOfGameObjects pickups;
	const auto num_pickups = 10;
	for(auto i = 0; i < num_pickups; i++)
	{
		const auto rand_index = GetRandomIndex(0, rooms.size()-1);
		const auto random_room = rooms[rand_index];		

		// Place the pickup in the center of the candidate room
		const auto positionInRoom = GetCenterOfRoom(*random_room, w, h);
		auto npc = std::shared_ptr<Pickup>(new Pickup(positionInRoom.GetX(), positionInRoom.GetY(), w, h, true, _eventManager, _settingsManager));
		pickups.push_back(npc);
	}
	return pickups;
}

/// <summary>
/// Create the level objects
/// </summary>
/// <returns>List of level game objects</returns>
ListOfGameObjects LevelManager::CreateLevel()
{
	// Read level settings
	const auto rows = _settingsManager.get_int("grid", "rows");
	const auto cols = _settingsManager.get_int("grid", "cols");
	const auto screenWidth = _settingsManager.get_int("global", "screen_width");
	const auto screenHeight = _settingsManager.get_int("global", "screen_height");
	const auto rowWidth = screenWidth / cols; 
	const auto rowHeight = screenHeight / rows;

	// Index Resources File
	_resourceManager.IndexResources();
	
	// This is the list of all game objects
	ListOfGameObjects objects;	
	
	// Generate the level's rooms
	auto rooms = level_generator::generate_level(_resourceManager, _settingsManager, _eventManager);

	// Setup each room
	for (const auto& room: rooms)
	{		
		// Initialize each room's initial settings
		room->LoadSettings(_settingsManager);

		// room's will want to know when the player moved for collision detection etc
		room->SubscribeToEvent(EventType::PlayerMovedEventType, _eventManager);
		
		// Add each room to the scene
		room->RaiseEvent(std::make_shared<AddGameObjectToCurrentSceneEvent>(std::dynamic_pointer_cast<Room>(room)), _eventManager);

		// Allow for room's settings to be modified while playing
		room->SubscribeToEvent(EventType::SettingsReloaded, _eventManager);

		// Add each room as a game object
		objects.push_back(dynamic_pointer_cast<GameObject>(room));
	}
		
	// Create the player
	const auto player = CreatePlayer(rooms, rowWidth/2, rowHeight/2);

	// Create the pickups
	const auto pickups = CreatePickups(rooms, rowWidth/2, rowHeight/2);

	// Setup the pickups
	for(const auto &pickup : pickups)
	{
		pickup->RaiseEvent(std::shared_ptr<AddGameObjectToCurrentSceneEvent>(new AddGameObjectToCurrentSceneEvent(std::dynamic_pointer_cast<Pickup>(pickup))), _eventManager);
		pickup->SubscribeToEvent(EventType::PlayerMovedEventType, _eventManager);
		pickup->SubscribeToEvent(gamelib::EventType::DoLogicUpdateEventType, _eventManager);
		
		objects.push_back(pickup);
		
	}
	
	// Add player to game world
	objects.push_back(player);

	// Set game objects to the game world
	gameWorld.objects = objects;	

	// Add the game world component on the player (each player can see in to the game world)
	player->AddComponent(std::make_shared<game_world_component>(gameWorld));
	
	return objects;
}
