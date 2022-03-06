#include "LevelManager.h"

using namespace gamelib;

bool IsSameSubscriber(IEventSubscriber* candidate, int subscriptionId) 
{
	if(candidate)
	{
		auto found_match = candidate == nullptr ? true : candidate->get_subscriber_id() == subscriptionId;
		return found_match;
	}
	return false;
};

// Handle level events
events LevelManager::handle_event(std::shared_ptr<event> evt)
{
	gamelib::events secondary_events;
	
	// Invalid move 
	if(evt->type == event_type::InvalidMove)
		gameCommands->InvalidMove();		
	
	// Recieved pickup
	if(evt->type == event_type::FetchedPickup)
		gameCommands->FetchedPickup();
	
	// Received per GameObject event
	if(evt->type == event_type::GameObject)
	{		
		const auto gameObjectEvent = dynamic_pointer_cast<GameObjectEvent>(evt);
		
		// Level Manager is responsible for removing game object from game
		if(gameObjectEvent->context == GameObjectEventContext::Remove)
		{
			RemoveGameObject(world, *gameObjectEvent->game_object );			
		}

	}
	return secondary_events;
}

void LevelManager::RemoveGameObject(GameWorld& gameWorld, GameObject& gameObject)
{
	auto &objects = world.game_objects;
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
				event_admin.Unsubscribe((*result)->id);

				// Erase from list of known game object
				objects.erase(result);
			}
}

void LevelManager::InitGameWorldData() const
{
	world.is_game_done = false;
	world.is_network_game = false;
	world.can_render = true;
}

LevelManager::LevelManager(
	EventManager& event_admin, 
	ResourceManager& resource_admin, 
	SettingsManager& settings_admin, 
	GameWorld& world, 
	SceneManager& scene_admin,
	AudioManager& audio_admin,
	logger& gameLogger) : event_admin(event_admin),
	                                         resource_admin(resource_admin),
	                                         settings_admin(settings_admin), 
	                                         world(world),
	                                         scene_admin(scene_admin),
											 audio_admin(audio_admin), gameLogger(gameLogger)
{
}

bool LevelManager::initialize()
{
	// set basic game world defaults
	InitGameWorldData();

	// All game commands that we handle in this game
	gameCommands = make_shared<GameCommands>(settings_admin, event_admin, audio_admin, resource_admin, world, gameLogger);

	// subscribe to game events
	event_admin.subscribe_to_event(event_type::GenerateNewLevel, this);
	event_admin.subscribe_to_event(event_type::InvalidMove, this);
	event_admin.subscribe_to_event(event_type::FetchedPickup, this);
	event_admin.subscribe_to_event(event_type::GameObject, this);
		
	return true;
}

// Read from SDL for keyboard data
void LevelManager::GetKeyboardInput()
{
	SDL_Event sdl_event;
	const auto bVerbose = settings_admin.get_bool("global", "verbose");
	
	while (SDL_PollEvent(&sdl_event) != 0)
	{
		// look for any key press
		if (sdl_event.type == SDL_KEYDOWN)
		{
			switch (sdl_event.key.keysym.sym)
			{
			case SDLK_SPACE:				
				gameCommands->Fire(bVerbose);
				break;
			case SDLK_w:
			case SDLK_UP:				
				gameCommands->MoveUp(bVerbose);
				break;
			case SDLK_s:
			case SDLK_DOWN:				
				gameCommands->MoveDown(bVerbose);
				break;
			case SDLK_a:
			case SDLK_LEFT:				
				gameCommands->MoveLeft(bVerbose);
				break;
			case SDLK_d:
			case SDLK_RIGHT:				
				gameCommands->MoveRight(bVerbose);
				break;
			case SDLK_q:
			case SDLK_ESCAPE:
				gameCommands->Quit(bVerbose);
				break;
			case SDLK_j:				
				gameCommands->ChangeLevel(bVerbose, 1);
				break;
			case SDLK_k:
				gameCommands->ChangeLevel(bVerbose, 2);
				break;
			case SDLK_l:
				gameCommands->ChangeLevel(bVerbose, 3);
				break;
			case SDLK_x:
				gameCommands->ChangeLevel(bVerbose, 4);
				break;
			case SDLK_1:
				audio_admin.play_sound(gamelib::AudioManager::to_resource(resource_admin.get("high.wav"))->as_fx());
				break;
			case SDLK_2:
				audio_admin.play_sound(gamelib::AudioManager::to_resource(resource_admin.get("medium.wav"))->as_fx());
				break;
			case SDLK_3:
				audio_admin.play_sound(gamelib::AudioManager::to_resource(resource_admin.get("low.wav"))->as_fx());
				break;
			case SDLK_4:
				audio_admin.play_sound(gamelib::AudioManager::to_resource(resource_admin.get("scratch.wav"))->as_fx());
				break;
			case SDLK_9:
				gameCommands->ToggleMusic(bVerbose);
				break;
			case SDLK_r:				
				gameCommands->ReloadSettings(bVerbose);
				break;
			case SDLK_g:				
				gameCommands->GenerateNewLevel(bVerbose);
				break;
			default:
				std::cout << "Unknown control key" << std::endl;
				log_message("Unknown control key", gameLogger, bVerbose);
				break;
			}
		}
		
		// quit
		if (sdl_event.type == SDL_QUIT)
		{
			gameCommands->Quit(bVerbose);
			return;
		}
	}
}

size_t LevelManager::get_random_index(const int min, const int max)
{
	return rand() % (max - min + 1) + min;
}

coordinate<int> GetCenterOfRoom(const Room &room, const int w, const int h)
{
	auto const room_x_mid = room.get_x() + (room.get_w() / 2);
	auto const room_y_mid = room.get_y() + (room.get_h() / 2);
	auto const x = room_x_mid - w /2;
	auto const y = room_y_mid - h /2;			
	return coordinate<int>(x, y);
}

shared_ptr<GameObject> LevelManager::CreatePlayer(const vector<shared_ptr<Room>> rooms, const int w, const int h) const
{	
	const auto minNumRooms = 0;
	const auto playerRoomIndex = get_random_index(minNumRooms, rooms.size());
	const auto playerRoom = rooms[playerRoomIndex];
	const auto positionInRoom = GetCenterOfRoom(*playerRoom, w, h);
	
	// create the player
	const auto player =  shared_ptr<Player>(new Player(positionInRoom.get_x(), positionInRoom.get_y(), w, h, settings_admin, event_admin, gameLogger));
	player->within_room_index = playerRoomIndex;

	player->set_tag(constants::playerTag);
	
	// Load in external settings for the player
	player->load_settings(settings_admin);

	// Player will respond to directional keyboard input
	player->subscribe_to_event(event_type::PositionChangeEventType, event_admin);

	// Player will be able to have its settings re-loaded at runtime
	player->subscribe_to_event(event_type::SettingsReloaded, event_admin);

	// Player can update itself 
	player->subscribe_to_event(event_type::DoLogicUpdateEventType, event_admin);

	// Add the player to the current scene/level
	player->raise_event(std::make_shared<add_game_object_to_current_scene_event>(player, 100), event_admin);

	// Place the player in a random room
	player->center_player_in_room(playerRoom);

	return player;
}




game_objects LevelManager::CreatePickups(const vector<shared_ptr<Room>>& rooms, const int w, const int h)
{
	game_objects pickups;
	const auto num_pickups = 10;
	for(auto i = 0; i < num_pickups; i++)
	{
		const auto rand_index = get_random_index(0, rooms.size()-1);
		const auto random_room = rooms[rand_index];		

		// Place the pickup in the center of the candidate room
		const auto positionInRoom = GetCenterOfRoom(*random_room, w, h);
		const auto npc = std::shared_ptr<Pickup>(new Pickup(positionInRoom.get_x(), positionInRoom.get_y(), w, h, true, event_admin, settings_admin));
		pickups.push_back(npc);
	}
	return pickups;
}

game_objects LevelManager::CreateLevel()
{
	// Read level settings
	const auto rows = settings_admin.get_int("grid", "rows");
	const auto cols = settings_admin.get_int("grid", "cols");
	const auto screenWidth = settings_admin.get_int("global", "screen_width");
	const auto screenHeight = settings_admin.get_int("global", "screen_height");
	const auto rowWidth = screenWidth / cols; 
	const auto rowHeight = screenHeight / rows;

	// Index Resources File
	resource_admin.IndexResources();
	
	// This is the list of all game objects
	game_objects objects;	
	
	// Generate the level's rooms
	auto rooms = level_generator::generate_level(resource_admin, settings_admin, event_admin);

	// Setup each room
	for (const auto& room: rooms)
	{		
		// Initialize each room's initial settings
		room->load_settings(settings_admin);

		// room's will want to know when the player moved for collision detection etc
		room->subscribe_to_event(event_type::PlayerMovedEventType, event_admin);
		
		// Add each room to the scene
		room->raise_event(std::make_shared<add_game_object_to_current_scene_event>(std::dynamic_pointer_cast<Room>(room)), event_admin);

		// Allow for room's settings to be modified while playing
		room->subscribe_to_event(event_type::SettingsReloaded, event_admin);

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
		pickup->raise_event(std::make_shared<add_game_object_to_current_scene_event>(std::dynamic_pointer_cast<Pickup>(pickup)), event_admin);
		pickup->subscribe_to_event(event_type::PlayerMovedEventType, event_admin);
		pickup->subscribe_to_event(gamelib::event_type::DoLogicUpdateEventType, event_admin);
		
		objects.push_back(pickup);
		
	}
	
	// Add player to game world
	objects.push_back(player);

	// Set game objects to the game world
	world.game_objects = objects;	

	// Add the game world component on the player (each player can see in to the game world)
	player->add_component(make_shared<class game_world_component>(world));
	
	return objects;
}




