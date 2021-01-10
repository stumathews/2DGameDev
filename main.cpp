#include <SDL.h>
#include <Windows.h>
#include <iostream>
#include "game/game_world.h"
#include "common/Common.h"
#include "game_structure.h"
#include "font/font_manager.h"
#include <events/event_manager.h>

#include "pickup.h"
#include "audio/AudioManager.h"
#include "common/constants.h"
#include "events/AddGameObjectToCurrentSceneEvent.h"
#include "events/PositionChangeEvent.h"
#include "events/SceneChangedEvent.h"
#include "game/LevelGenerator.h"
#include "game/exceptions/game_exception.h"
#include "graphic/sdl_graphics_manager.h"
#include "resource/resource_manager.h"
#include "scene/scene_manager.h"
#include "util/settings_manager.h"
#include "objects/game_world_component.h"

using namespace std;
using namespace gamelib;




class level_manager : event_subscriber
{
public:
	std::string get_subscriber_name() override { return "level_manager";};
	void get_input();
	std::vector<std::shared_ptr<event>> handle_event(std::shared_ptr<event> evt) override;
	void init_game_world_data() const;
	level_manager(std::shared_ptr<event_manager> event_admin,
	                                             shared_ptr<resource_manager> resource_admin,
	                                             shared_ptr<settings_manager> settings_admin, 
												 shared_ptr<game_world> world,
												 shared_ptr<scene_manager> scene_admin, shared_ptr<audio_manager> audio_admin);
	bool initialize();
	size_t get_random_index(const int min, const int max) const;

	shared_ptr<game_object> create_player(vector<shared_ptr<square>> rooms, const int w, const int h) const;
	vector<shared_ptr<square>> create_pickups(const vector<shared_ptr<square>>& rooms, const int w, const int h);
	vector<shared_ptr<game_object>> load_content();
	std::shared_ptr<event_manager> event_admin;
	std::shared_ptr<resource_manager> resource_admin;
	std::shared_ptr<settings_manager> settings_admin;
	shared_ptr<game_world> world;
	shared_ptr<scene_manager> scene_admin;
	shared_ptr<audio_manager> audio_admin;
};

void level_manager::get_input()
{
	SDL_Event sdl_event;
	const auto be_verbose = settings_admin->get_bool("global", "verbose");
	while(SDL_PollEvent(&sdl_event) != 0)
	{
		if(sdl_event.type == SDL_QUIT)
		{
			world->is_game_done = true;
			return;
		}
		
		if (sdl_event.type == SDL_KEYDOWN)
		{
			switch (sdl_event.key.keysym.sym)
			{
			case SDLK_SPACE:
				run_and_log("Space bar pressed!", be_verbose, [&]()
				{
					event_admin->raise_event(make_shared<event>(event_type::Fire), this);
					return true;
				}, true, true, settings_admin);
				break;
			case SDLK_w:
			case SDLK_UP:
				run_and_log("Player pressed up!", be_verbose, [&]()
				{
					event_admin->raise_event(std::make_unique<position_change_event>(Direction::Up), this);
					return true;
				}, true, true, settings_admin);
				break;
			case SDLK_s:
			case SDLK_DOWN:
				run_and_log("Player pressed down!", be_verbose, [&]()
				{
					event_admin->raise_event(std::make_unique<position_change_event>(Direction::Down), this);
					return true;
				}, true, true, settings_admin);
				break;
			case SDLK_a:
			case SDLK_LEFT:
				run_and_log("Player pressed left!", be_verbose, [&]()
				{
					event_admin->raise_event(std::make_unique<position_change_event>(Direction::Left), this);
					return true;
				}, true, true, settings_admin);
				break;

			case SDLK_d:
			case SDLK_RIGHT:
				run_and_log("Player pressed right!", be_verbose, [&]()
				{
					event_admin->raise_event(std::make_unique<position_change_event>(Direction::Right), this);
					return true;
				}, true, true, settings_admin);
				break;

			case SDLK_q:
			case SDLK_ESCAPE:
				run_and_log("Player pressed quit!", be_verbose, [&]()
				{
					world->is_game_done = 1;
					return true;
				}, true, true, settings_admin);
				break;
			case SDLK_j:
				run_and_log("Change to level 1", be_verbose, [&]()
				{
					event_admin->raise_event(std::make_unique<scene_changed_event>(1), this);
					return true;
				}, true, true, settings_admin);
				break;
			case SDLK_k:
				run_and_log("Change to level 2", be_verbose, [&]()
				{
					event_admin->raise_event(std::make_unique<scene_changed_event>(2), this);
					return true;
				}, true, true, settings_admin);
				break;
			case SDLK_l:
				run_and_log("Change to level 3", be_verbose, [&]()
				{
					event_admin->raise_event(std::make_unique<scene_changed_event>(3), this);
					return true;
				}, true, true, settings_admin);
				break;

			case SDLK_x:
				run_and_log("Change to level 4", be_verbose, [&]()
				{
					event_admin->raise_event(std::make_unique<scene_changed_event>(4), this);
					return true;
				}, true, true, settings_admin);
				break;
			case SDLK_1:
				audio_admin->play_sound(audio_manager::to_resource(resource_admin->get("high.wav"))->as_fx());						
				break;
			case SDLK_2:
				audio_admin->play_sound(audio_manager::to_resource(resource_admin->get("medium.wav"))->as_fx());	
				break;
			case SDLK_3:
				audio_admin->play_sound(audio_manager::to_resource(resource_admin->get("low.wav"))->as_fx());	
				break;
			case SDLK_4:
				audio_admin->play_sound(audio_manager::to_resource(resource_admin->get("scratch.wav"))->as_fx());	
				break;
			case SDLK_9:
				if (Mix_PlayingMusic() == 0)
				{
					audio_admin->play_music(audio_manager::to_resource(resource_admin->get("MainTheme.wav"))->as_music());					
				}
				else
				{
					if (Mix_PausedMusic() == 1)
						Mix_ResumeMusic();
					else
						Mix_PauseMusic();
				}
				break;
			case SDLK_0:
				Mix_HaltMusic();
				break;
			case SDLK_r:
				settings_admin->reload();
				event_admin->raise_event(make_shared<event>(event_type::SettingsReloaded), this);
				log_message("Settings reloaded", be_verbose, false);
				break;
			case SDLK_g:						
				event_admin->raise_event(make_shared<event>(event_type::GenerateNewLevel), this);
				log_message("Generating new level", be_verbose, false);
				break;
			default:
				std::cout << "Unknown control key" << std::endl;
				log_message("Unknown control key", be_verbose);
				break;
			}
		}
		
	}
}

events level_manager::handle_event(std::shared_ptr<event> evt)
{
	events secondary_events;
	
	if(evt->type == event_type::InvalidMove)
	{		
		audio_admin->play_sound(audio_manager::to_resource(resource_admin->get(settings_admin->get_string("audio","invalid_move")))->as_fx());	
	}
		
	return secondary_events;
}

void level_manager::init_game_world_data() const
{
	world->is_game_done = false;
	world->is_network_game = false;
	world->can_render = true;
}

level_manager::level_manager(std::shared_ptr<event_manager> event_admin, shared_ptr<resource_manager> resource_admin, 
	shared_ptr<settings_manager> settings_admin, shared_ptr<game_world> world, shared_ptr<scene_manager> scene_admin,
	shared_ptr<audio_manager> audio_admin) : event_admin(std::move(event_admin)),
	                                         resource_admin(std::move(resource_admin)),
	                                         settings_admin(std::move(settings_admin)), world(std::move(world)),
	                                         scene_admin(std::move(scene_admin)),
	                                         audio_admin(std::move(audio_admin))
{
	
}

bool level_manager::initialize()
{
	init_game_world_data();

	// Let te level manager re-generate a level if a request to do so is made
	event_admin->subscribe_to_event(event_type::GenerateNewLevel, this);

	// The level manager will play the appropriate sound, update invalid move statics etc
	event_admin->subscribe_to_event(event_type::InvalidMove, this);
	
	return true;
}

size_t level_manager::get_random_index(const int min, const int max) const
{
	return rand() % (max - min + 1) + min;
}

shared_ptr<game_object> level_manager::create_player(const vector<shared_ptr<square>> rooms, const int w, const int h) const
{
	
	const auto min = 0;
	const auto random_room_index = get_random_index(min, rooms.size());
	const auto random_room = rooms[random_room_index];
	
	// create the player
	const auto the_player =  make_shared<player>(player(0, 0, w, h, resource_admin, settings_admin, event_admin));

	the_player->set_tag(constants::playerTag);
	
	// Load in external settings for the player
	the_player->load_settings(settings_admin);

	// Player will respond to directional keyboard input
	the_player->subscribe_to_event(event_type::PositionChangeEventType, event_admin);

	// Player will be able to have its settings re-loaded at runtime
	the_player->subscribe_to_event(event_type::SettingsReloaded, event_admin);

	// Add the player to te current scene/level
	the_player->raise_event(std::make_shared<add_game_object_to_current_scene_event>(the_player, 100), event_admin);

	// Place the player in a random room
	the_player->center_player_in_room(random_room);

	return the_player;
}


coordinate<int> center_within(const square &room, const int w, const int h)
{
	auto const room_x_mid = room.get_x() + (room.get_w() / 2);
		auto const room_y_mid = room.get_y() + (room.get_h() / 2);
		auto const x = room_x_mid - w /2;
		auto const y = room_y_mid - h /2;			
		return coordinate<int>(x, y);
}

vector<shared_ptr<square>> level_manager::create_pickups(const vector<shared_ptr<square>>& rooms, const int w, const int h)
{
	vector<shared_ptr<square>> pickups;
	const auto num_pickups = 10;
	for(auto i = 0; i < num_pickups; i++)
	{
		const auto rand_index = get_random_index(0, rooms.size());
		const auto random_room = rooms[rand_index];		
		const auto center = center_within(*random_room, w, h);
		const auto npc = make_shared<pickup>(i, center.get_x(), center.get_y(), w, h, resource_admin, true, false, true, settings_admin);
		npc->set_fill(true);
		pickups.push_back(npc);
	}
	return pickups;
}

vector<shared_ptr<game_object>> level_manager::load_content()
{
	resource_admin->read_resources();
	
	game_objects objects;	
	
	// Generate the level's rooms
	auto rooms = level_generator::generate_level(resource_admin, settings_admin);
	for (const auto& room: rooms)
	{			
		// room's will want to know when the player moved for collision detection etc
		room->subscribe_to_event(event_type::PlayerMovedEventType, event_admin);
		
		// Add each room to the scene
		room->raise_event(std::make_shared<add_game_object_to_current_scene_event>(std::dynamic_pointer_cast<square>(room)), event_admin);

		// Initialize each room's initial settings
		room->load_settings(settings_admin);

		// Allow for room's settings to be modified while playing
		room->subscribe_to_event(event_type::SettingsReloaded, event_admin);

		// Add each room as a game object
		objects.push_back( dynamic_pointer_cast<game_object>(room));
	}
		
	// Create the player
	const auto rows = settings_admin->get_int("grid","rows");
	const auto cols = settings_admin->get_int("grid","cols");
	const auto screen_width = settings_admin->get_int("global","screen_width");
	const auto screen_height = settings_admin->get_int("global","screen_height");
	const auto w = screen_width / cols; 
	const auto h = screen_height / rows;
	const auto player = create_player(rooms, w/2, h/2);

	const auto pickups = create_pickups(rooms, w/2, h/2);
	for(const auto &pickup : pickups)
	{
		pickup->raise_event(std::make_shared<add_game_object_to_current_scene_event>(std::dynamic_pointer_cast<square>(pickup)), event_admin);
		pickup->subscribe_to_event(event_type::PlayerMovedEventType, event_admin);
		objects.push_back(pickup);
	}
	
	// Add player to game world
	objects.push_back(player);

	// Add game objects to the game world
	world->game_objects = objects;	

	// Add the game world component on the player
	player->add_component(make_shared<class game_world_component>(world));
	
	return objects;
}

int main(int argc, char *args[])
{
	try
	{
		const auto world = make_shared<game_world>();		
		const auto settings_admin = make_shared<settings_manager>();
		const auto event_admin = make_shared<event_manager>(settings_admin);
		const auto graphics_admin = make_shared<sdl_graphics_manager>(event_admin);
		const auto audio_admin = make_shared<audio_manager>();
		const auto font_admin = make_shared<font_manager>();
		const auto resource_admin = make_shared<resource_manager>(settings_admin, graphics_admin, font_admin, audio_admin); 
		const auto scene_admin = make_shared<scene_manager>(event_admin, settings_admin, resource_admin);		
		const auto level_admin = make_shared<level_manager>(event_admin, resource_admin, settings_admin, world, scene_admin, audio_admin);
		const auto structure =  make_shared<game_structure>(event_admin, resource_admin, settings_admin, world, scene_admin, graphics_admin, audio_admin, [&](){level_admin->get_input();});
		
		// Game Initialization
		
		const auto is_initialized = succeeded(structure->initialize(), "Initialize");
		const auto is_loaded = succeeded(level_admin->initialize(), "loading content");

		// Create the level
		level_admin->load_content();

		if(!is_initialized || !is_loaded)
			return -1;

		// Main Game Loop
		
		const auto loop_result = run_and_log("Beginning game loop...", settings_admin->get_bool("global","verbose"), [&]()
		{
			return structure->game_loop();
		}, true, true, settings_admin);

		// Unload Game
		
		const auto unload_result = run_and_log("Unloading game...", settings_admin->get_bool("global","verbose"), [&]()
		{
			return structure->unload();
		}, true, true, settings_admin);
		
		return succeeded(loop_result, "game loop failed") && 
			   succeeded(unload_result, "content unload failed");
	
	}
	catch(game_exception& game_exception)
	{
		log_message("Game Exception occurred from " + string(game_exception.get_source_subsystem()) + string(": ") + string(game_exception.what()));
	}
	return 0;
}
