
#include <SDL.h>
#include <SDL_image.h>
#include <Windows.h>
#include <iostream>
#include "Common.h"
#include <SDL_mixer.h>
#include "Events.h"
#include "resource_manager.h"
#include "SceneChangedEvent.h"
#include "scene_manager.h"
#include "DoLogicUpdateEvent.h"
#include "RenderManager3D.h"
#include <SDL_ttf.h>
#include "global_config.h"
#include "game_structure.h"
#include <functional>
#include "player_moved_event.h"
#include "AddGameObjectToCurrentSceneEvent.h"
#include "LevelGenerator.h"
#include "AudioResource.h"
#include "AudioManager.h"

using namespace std;

extern shared_ptr<event_manager> event_admin;
extern shared_ptr<resource_manager> resource_admin;
extern shared_ptr<global_config> config;
extern shared_ptr<game_world_data> game_world;
extern shared_ptr<scene_manager> scene_admin;

void game_structure::get_input()
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
					run_and_log("Player pressed up!", config->verbose, [&]()
					{
						event_admin->raise_event(std::make_unique<position_change_event>(Direction::Up), this);
						return true;
					});
					break;
				case SDLK_s:
				case SDLK_DOWN:
					run_and_log("Player pressed down!", config->verbose, [&]()
					{
						event_admin->raise_event(std::make_unique<position_change_event>(Direction::Down), this);
						return true;
					});
					break;
				case SDLK_a:
				case SDLK_LEFT:
					run_and_log("Player pressed left!", config->verbose, [&]()
					{
						event_admin->raise_event(std::make_unique<position_change_event>(Direction::Left), this);
						return true;
					});
					break;

				case SDLK_d:
				case SDLK_RIGHT:
					run_and_log("Player pressed right!", config->verbose, [&]()
					{
						event_admin->raise_event(std::make_unique<position_change_event>(Direction::Right), this);
						return true;
					});
					break;

				case SDLK_q:
					run_and_log("Player pressed quit!", config->verbose, [&]()
					{
						game_world->is_game_done = 1;
						return true;
					});
					break;
				case SDLK_j:
					run_and_log("Change to level 1", config->verbose, [&]()
					{
						event_admin->raise_event(std::make_unique<scene_changed_event>(1), this);
						return true;
					});
					break;
				case SDLK_k:
					run_and_log("Change to level 2", config->verbose, [&]()
					{
						event_admin->raise_event(std::make_unique<scene_changed_event>(2), this);
						return true;
					});
					break;
				case SDLK_l:
					run_and_log("Change to level 3", config->verbose, [&]()
					{
						event_admin->raise_event(std::make_unique<scene_changed_event>(3), this);
						return true;
					});
					break;

				case SDLK_x:
					run_and_log("Change to level 4", config->verbose, [&]()
					{
						event_admin->raise_event(std::make_unique<scene_changed_event>(4), this);
						return true;
					});
					break;
				case SDLK_1:
					Mix_PlayChannel(-1, audio_manager::to_resource(resource_admin->get("high.wav"))->as_fx(), 0);
					break;
				case SDLK_2:
					Mix_PlayChannel(-1, audio_manager::to_resource(resource_admin->get("medium.wav"))->as_fx(), 0);
					break;
				case SDLK_3:
					Mix_PlayChannel(-1,  audio_manager::to_resource(resource_admin->get("low.wav"))->as_fx(), 0);
					break;
				case SDLK_4:
					Mix_PlayChannel(-1, audio_manager::to_resource(resource_admin->get("scratch.wav"))->as_fx(), 0);
					break;
				case SDLK_9:
					if (Mix_PlayingMusic() == 0)
					{
						Mix_PlayMusic(audio_manager::to_resource(resource_admin->get("MainTheme.wav"))->as_music(), -1);
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

				default:
					std::cout << "Unknown control key" << std::endl;
					log_message("Unknown control key", config->verbose);
					break;
				}
			}
		}
		else
		{
			game_world->is_game_done = true;
		}
	}
}

/***
 * Keeps and updated snapshot of the player state
 */
void game_structure::player_update()
{
	// Read from game controller
	get_input();	
}


vector<shared_ptr<event>> game_structure::process_event(std::shared_ptr<event> evt)
{
	// game_structure does not subscribe to any events
	return vector<shared_ptr<event>>();
}

string game_structure::get_subscriber_name()
{
	return "game_structure";
}

/***
 * Updates, monitors what the world is doing around the player. This is usually what the player reacts to
 */
void game_structure::world_update()
{
	// Ask the event manager to notify event subscribers to update their logic now
	event_admin->raise_event(make_unique<do_logic_update_event>(), this);
}

// This is basically the update functions which is run x FPS to maintain a timed series on constant updates 
// that simulates constant movement for example or time intervals in a non-time related game (turn based game eg)
// This is where you would make state changes in the game such as decreasing ammo etc
void game_structure::update()
{	
	// This game logic keeps the world simulator running:
	player_update();
	
	// make the game do something now...show game activity that the user will then respond to
	// this generates game play
	world_update();
}

// Gets time in milliseconds now
long game_structure::get_tick_now()
{
	return timeGetTime();
}

void game_structure::spare_time(long frame_time)
{
	event_admin->process_all_events();
}

void game_structure::draw(float percent_within_tick)
{
	if(config->use_3d_render_manager)
		D3DRenderManager::GetInstance().update();
	else
		sdl_graphics_manager::get_instance().draw_current_scene(false);
}

bool game_structure::initialize_sdl(const int screen_width, const int screen_height)
{
	return log_if_false(sdl_graphics_manager::get_instance().initialize(screen_width, screen_height),"Failed to initialize SDL graphics manager");
}

/**
 * Frees static content not managed by resource manager
 */
void game_structure::unload()
{
	resource_admin->unload();		
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();		
}


game_structure::game_structure()
{
	init_game_world_data();
}

void game_structure::init_game_world_data()
{
	game_world->is_game_done = false;
	game_world->is_network_game = false;
	game_world->can_render = true;
}

bool game_structure::initialize(int screen_width, int screen_height)
{
	return run_and_log("game_structure::initialize()", config->verbose, [&]()
	{
		resource_admin->initialize();

		const auto sdl_ok = log_if_false(initialize_sdl(screen_width, screen_height), "Could not initialize SDL, aborting.");
		
		if(!sdl_ok)
			return false;
		
		if(config->use_3d_render_manager)
			init3d_render_manager();

		return true;
	});
}

bool game_structure::init3d_render_manager()
{
	auto& renderManager = D3DRenderManager::GetInstance();
	renderManager.Initialize(GetModuleHandle(NULL), 800, 600, false, "My Window");
	auto mesh = new Mesh3D();
	mesh->create();
	D3DRenderManager::GetInstance().meshes.push_back(mesh);
	return true;
}

shared_ptr<player> game_structure::create_player() const
{
	return make_shared<player>(player(config->player_init_pos_x, config->player_init_pos_y,
	                                  config->square_width / 2));
}

void game_structure::setup_player() const
{
	const auto the_player = create_player();	
	the_player->subscribe_to_event(event_type::PositionChangeEventType);	
	the_player->raise_event(std::make_shared<add_game_object_to_current_scene_event>(the_player, 100));
}

/**
 Initializes resource manager and SDL
 */
bool game_structure::initialize()
{
	return run_and_log("game_structure::initialize()", config->verbose, [&]()
	{
		// Initialize resource manager
		const auto resource_admin_initialized_ok = log_if_false(resource_admin->initialize(), "Could not initialize resource manager");

		// Initialize event manager
		const auto event_admin_initialized_ok = log_if_false(event_admin->initialize(), "Could not initialize event manager");

		// Initialize scene_manager
		const auto scene_admin_initialized_ok = log_if_false(scene_admin->initialize(), "Could not initialize scene manager");

		// Initialize SDL
		const auto sdl_initialize_ok = log_if_false(initialize_sdl(config->screen_width, config->screen_height), "Could not initialize SDL, aborting.");
		const auto dx_render_manager_initialized_ok = config->use_3d_render_manager && init3d_render_manager(); // we dont use 3d yet
		
		if(failed(sdl_initialize_ok) || 
		   failed(event_admin_initialized_ok) ||
		   failed(resource_admin_initialized_ok) || 
		   failed(dx_render_manager_initialized_ok, "Ignoring dx renderer for now", true) || 
		   failed(scene_admin_initialized_ok)) 
			return false;		

		return true;
	});
}

/**
 * Update & Draw until the game ends
 */
void game_structure::game_loop()
{
	auto tick_count_at_last_call = get_tick_now();
	const auto max_loops = config->max_loops;

	// MAIN GAME LOOP!!
	while (!game_world->is_game_done) 
	{
		const auto new_time =  get_tick_now();
		auto frame_ticks = 0;  // Number of ticks in the update call	
		auto num_loops = 0;  // Number of loops ??
		auto ticks_since = new_time - tick_count_at_last_call;

		// New frame, happens consistently every 50 milliseconds. Ie 20 times a second.
		// 20 times a second = 50 milliseconds
		// 1 second is 20*50 = 1000 milliseconds
		while (ticks_since > config->tick_time_ms && num_loops < max_loops)
		{
			update();		
			tick_count_at_last_call += config->tick_time_ms; // tickCountAtLastCall is now been +Single<GlobalConfig>().TickTime more since the last time. update it
			frame_ticks += config->tick_time_ms; num_loops++;
			ticks_since = new_time - tick_count_at_last_call;
		}

		spare_time(frame_ticks); // handle player input, general housekeeping (Event Manager processing)

		if (game_world->is_network_game || ticks_since <= config->tick_time_ms)
		{
			if (game_world->can_render)
			{
				const auto percent_outside_frame = static_cast<float>(ticks_since / config->tick_time_ms) * 100; // NOLINT(bugprone-integer-division)				
				draw(percent_outside_frame);
			}
		}
		else
		{
			tick_count_at_last_call = new_time - config->tick_time_ms;
		}
	}
	std::cout << "Game done" << std::endl;
}

/**
 * Parses game resources, generates level and adds player to scene
 */
bool game_structure::load_content() const
{
	resource_admin->parse_game_resources();
		
	// Generate the level's rooms
	for (const auto& room: level_generator::generate_level())
	{	
		// room's will want to know when the player moved
		room->subscribe_to_event(event_type::PlayerMovedEventType);
		
		// Add each room to the scene
		room->raise_event(std::make_shared<add_game_object_to_current_scene_event>(std::dynamic_pointer_cast<square>(room)));	
	}

	// Create the player
	setup_player();

	return true;
}


