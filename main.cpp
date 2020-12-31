#include <SDL.h>
#include <Windows.h>
#include <iostream>
#include "game/game_world.h"
#include "common/Common.h"
#include "game_structure.h"
#include "font/font_manager.h"
#include <events/event_manager.h>
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

	shared_ptr<player> create_player(shared_ptr<settings_manager> settings_admin,
	                                 shared_ptr<resource_manager> resource_admin) const;
	shared_ptr<game_object> setup_player(vector<shared_ptr<square>> rooms) const;
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
		
		while(SDL_PollEvent(&sdl_event) != 0)
		{
			if(sdl_event.type != SDL_QUIT)
			{
				if (sdl_event.type == SDL_KEYDOWN)
				{
					switch (sdl_event.key.keysym.sym)
					{
					case SDLK_w:
					case SDLK_UP:
						run_and_log("Player pressed up!", settings_admin->get_bool("global", "verbose"), [&]()
						{
							event_admin->raise_event(std::make_unique<position_change_event>(Direction::Up), this);
							return true;
						}, true, true, settings_admin);
						break;
					case SDLK_s:
					case SDLK_DOWN:
						run_and_log("Player pressed down!", settings_admin->get_bool("global", "verbose"), [&]()
						{
							event_admin->raise_event(std::make_unique<position_change_event>(Direction::Down), this);
							return true;
						}, true, true, settings_admin);
						break;
					case SDLK_a:
					case SDLK_LEFT:
						run_and_log("Player pressed left!", settings_admin->get_bool("global", "verbose"), [&]()
						{
							event_admin->raise_event(std::make_unique<position_change_event>(Direction::Left), this);
							return true;
						}, true, true, settings_admin);
						break;

					case SDLK_d:
					case SDLK_RIGHT:
						run_and_log("Player pressed right!", settings_admin->get_bool("global", "verbose"), [&]()
						{
							event_admin->raise_event(std::make_unique<position_change_event>(Direction::Right), this);
							return true;
						}, true, true, settings_admin);
						break;

					case SDLK_q:
					case SDLK_ESCAPE:
						run_and_log("Player pressed quit!", settings_admin->get_bool("global", "verbose"), [&]()
						{
							world->is_game_done = 1;
							return true;
						}, true, true, settings_admin);
						break;
					case SDLK_j:
						run_and_log("Change to level 1", settings_admin->get_bool("global", "verbose"), [&]()
						{
							event_admin->raise_event(std::make_unique<scene_changed_event>(1), this);
							return true;
						}, true, true, settings_admin);
						break;
					case SDLK_k:
						run_and_log("Change to level 2", settings_admin->get_bool("global", "verbose"), [&]()
						{
							event_admin->raise_event(std::make_unique<scene_changed_event>(2), this);
							return true;
						}, true, true, settings_admin);
						break;
					case SDLK_l:
						run_and_log("Change to level 3", settings_admin->get_bool("global", "verbose"), [&]()
						{
							event_admin->raise_event(std::make_unique<scene_changed_event>(3), this);
							return true;
						}, true, true, settings_admin);
						break;

					case SDLK_x:
						run_and_log("Change to level 4", settings_admin->get_bool("global", "verbose"), [&]()
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
						log_message("Settings reloaded", settings_admin->get_bool("global", "verbose"), false);
						break;
					case SDLK_g:						
						event_admin->raise_event(make_shared<event>(event_type::GenerateNewLevel), this);
						log_message("Generating new level", settings_admin->get_bool("global", "verbose"), false);
						break;
					default:
						std::cout << "Unknown control key" << std::endl;
						log_message("Unknown control key", settings_admin->get_bool("global", "verbose"));
						break;
					}
				}
			}
			else
			{
				world->is_game_done = true;
			}
		}
	}

events level_manager::handle_event(std::shared_ptr<event> evt)
{
	events secondary_events;
	if(evt->get_type() == event_type::GenerateNewLevel)
	{
		load_content();
	}
	
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

level_manager::level_manager(std::shared_ptr<event_manager> event_admin,
	std::shared_ptr<resource_manager> resource_admin, std::shared_ptr<settings_manager> settings_admin, shared_ptr<game_world> world, shared_ptr<scene_manager> scene_admin, shared_ptr<audio_manager> audio_admin)
	: event_admin(std::move(event_admin)), resource_admin(std::move(resource_admin)), settings_admin(std::move(settings_admin)), world(std::move(world)), scene_admin(std::move(scene_admin)), audio_admin(std::move(audio_admin))
{
	
}

bool level_manager::initialize()
{
	init_game_world_data();
	
	event_admin->subscribe_to_event(event_type::GenerateNewLevel, this);
	event_admin->subscribe_to_event(event_type::InvalidMove, this);
	return true;
}

shared_ptr<player> level_manager::create_player(shared_ptr<settings_manager> settings_admin, shared_ptr<resource_manager> resource_admin) const
{
	const auto rows = settings_admin->get_int("grid","rows");
	const auto cols = settings_admin->get_int("grid","cols");
	const auto screen_width = settings_admin->get_int("global","screen_width");
	const auto screen_height = settings_admin->get_int("global","screen_height");
	const auto w = screen_width / cols; //settings_admin->get_int("global","square_width");
	const auto h = screen_height / rows;
	auto const x = settings_admin->get_int("player","player_init_pos_x");
	auto const y = settings_admin->get_int("player", "player_init_pos_y");
	
	
	return make_shared<player>(player(x, y, w, h, resource_admin, settings_admin, event_admin));
}

shared_ptr<game_object> level_manager::setup_player(vector<shared_ptr<square>> rooms) const
{
	const auto the_player = create_player(settings_admin, resource_admin);
	const auto room_component = make_shared<component>("rooms");
	
	the_player->load_settings(settings_admin);

	the_player->subscribe_to_event(event_type::PositionChangeEventType, event_admin);			
	the_player->subscribe_to_event(event_type::SettingsReloaded, event_admin);


	the_player->set_tag(constants::playerTag);
	the_player->add_component(room_component);

	the_player->raise_event(std::make_shared<add_game_object_to_current_scene_event>(the_player, 100), event_admin);

	const auto min = 0;
	const auto random_room_index = rand() % (rooms.size()-min+1) + min;
	the_player->set_room_within(random_room_index);

	return the_player;
}

vector<shared_ptr<game_object>> level_manager::load_content()
{	
	resource_admin->read_resources();

	vector<shared_ptr<game_object>> game_objects;
	
	
	// Generate the level's rooms
	auto rooms = level_generator::generate_level(resource_admin, settings_admin);
	for (const auto& room: rooms)
	{			
		// room's will want to know when the player moved for collision detection etc
		room->subscribe_to_event(event_type::PlayerMovedEventType, event_admin);
		
		// Add each room to the scene
		room->raise_event(std::make_shared<add_game_object_to_current_scene_event>(std::dynamic_pointer_cast<square>(room)), event_admin);
					
		room->load_settings(settings_admin);
		room->subscribe_to_event(event_type::SettingsReloaded, event_admin);

		game_objects.push_back( dynamic_pointer_cast<game_object>(room));
	}
	
	
	// Create the player
	const auto player = setup_player(rooms);
	

	// Add player to game world
	game_objects.push_back(player);

	// Setup game world
	world->game_objects = game_objects;	
	const auto game_world_component = make_shared<class game_world_component>(world);

	// Add the game world as a component on the player
	player->add_component(game_world_component);
	
	return game_objects;
}

int main(int argc, char *args[])
{
	try
	{
		const auto world = make_shared<game_world>();		
		const auto settings_admin = make_shared<settings_manager>();
		const auto event_admin = make_shared<event_manager>(settings_admin);
		const auto graphics_admin = make_shared<sdl_graphics_manager>(event_admin);
		const auto audio_admin = make_shared<audio_manager>(); // makes audio resources
		const auto font_admin = make_shared<font_manager>(); // makes font resources
		const auto resource_admin = make_shared<resource_manager>(settings_admin, graphics_admin, font_admin, audio_admin); 
		const auto scene_admin = make_shared<scene_manager>(event_admin, settings_admin, resource_admin);
		
		const auto level_admin = make_shared<level_manager>(event_admin, resource_admin, settings_admin, world, scene_admin, audio_admin);
		const auto structure =  make_shared<game_structure>(event_admin, resource_admin, settings_admin, world, scene_admin, graphics_admin, audio_admin, [&](){level_admin->get_input();});
		
		
		

		// Game Initialization
		
		const auto is_initialized = succeeded(structure->initialize(), "Initialize");
		const auto is_loaded = succeeded(level_admin->initialize(), "loading content");

		
		level_admin->load_content();

		if(!is_initialized || !is_loaded)
			return -1;

		// Game Loop
		
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
