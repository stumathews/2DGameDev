
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

using namespace std;

extern shared_ptr<event_manager> event_admin;
extern shared_ptr<resource_manager> resource_admin;
extern shared_ptr<global_config> config;


// Create our global copy of the game data
std::shared_ptr<game_world_data> g_pGameWorldData = std::make_shared<game_world_data>();

void game_structure::get_input()
{
	const auto be_verbose = config->verbose;	
	SDL_Event e;
	
	while(SDL_PollEvent(&e) != 0)
	{
		if(e.type == SDL_QUIT) 
		{
			g_pGameWorldData->is_game_done = true;
		}
		else if( e.type == SDL_KEYDOWN ) 
		{
			switch( e.key.keysym.sym )
			{
				case SDLK_w:
				case SDLK_UP:
					run_and_log("Player pressed up!", be_verbose, [&]()
					{
						event_admin->raise_event(std::make_unique<position_change_event>(Up), this);
						return true;
					});
				break;
				case SDLK_s:
				case SDLK_DOWN:
					run_and_log("Player pressed down!", be_verbose, [&]()
					{
						event_admin->raise_event(std::make_unique<position_change_event>(Down), this);
						return true;
					});
				break;
				case SDLK_a:
				case SDLK_LEFT:
					run_and_log("Player pressed left!", be_verbose, [&]()
					{				
						event_admin->raise_event(std::make_unique<position_change_event>(Left), this);
						return true;
					});
				break;

				case SDLK_d:
				case SDLK_RIGHT:
					run_and_log("Player pressed right!", be_verbose, [&]()
					{	
						event_admin->raise_event(std::make_unique<position_change_event>(Right), this);
						return true;
					});
				break;

				case SDLK_q:
					run_and_log("Player pressed quit!", be_verbose, [&]()
					{	
						g_pGameWorldData->is_game_done = 1;
						return true;
					});
					break;
				case SDLK_j:
					run_and_log("Change to level 1", be_verbose, [&]()
					{	
						event_admin->raise_event(std::make_unique<scene_changed_event>(1), this);
						return true;
					});
					break;
				case SDLK_k:
					run_and_log("Change to level 2", be_verbose, [&]()
					{
						event_admin->raise_event(std::make_unique<scene_changed_event>(2), this);
						return true;
					});
				break;
				case SDLK_l:
					run_and_log("Change to level 3", be_verbose, [&]()
					{
						event_admin->raise_event(std::make_unique<scene_changed_event>(3),this);
						return true;
					});
				break;

				case SDLK_x:
					run_and_log("Change to level 4", be_verbose, [&]()
					{
						event_admin->raise_event(std::make_unique<scene_changed_event>(4), this);
						return true;
					});
				break;	
                case SDLK_1:
					Mix_PlayChannel( -1, config->high_sound_fx, 0 );
                break;                            
                case SDLK_2:
					Mix_PlayChannel( -1, config->medium_sound_fx, 0 );
                break;
                case SDLK_3:
					Mix_PlayChannel( -1, config->low_sound_fx, 0 );
                break;
                case SDLK_4:
					Mix_PlayChannel( -1, config->scratch_fx, 0 );
                break;
				case SDLK_9:
					if( Mix_PlayingMusic() == 0 ) {
						Mix_PlayMusic( config->music, -1 );
					} else 	{
						if(Mix_PausedMusic() == 1)
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
					log_message("Unknown control key", be_verbose);
				break;
			}
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

void game_structure::update_state()
{
	// Ask the event manager to notify event subscribers to update their logic now
	event_admin->raise_event(make_unique<do_logic_update_event>(), this);
}

/***
 * Update simple logical elements such as doors, elevators or moving platforms
 */
void game_structure::update_logic_based_elements()
{	
	update_state();
}

vector<shared_ptr<event>> game_structure::process_event(std::shared_ptr<event> evt)
{
	return vector<shared_ptr<event>>();
}

string game_structure::get_subscriber_name()
{
	return "game_structure";
}

/***
 * Update active elements such as decorative flying birds or doors that open and close
 */
void game_structure::update_active_elements()
{
	update_logic_based_elements(); // doors, elevators, movng platforms, real enemies with a distinctive behavior (simple)
}

/***
 * Updates, monitors what the world is doing around the player. This is usually what the player reacts to
 */
void game_structure::world_update()
{
	update_active_elements(); // flying birds, doors that open and close - must be checked to keep consistent, meaningful experiance
}

// This is basically the update functions which is run x FPS to maintain a timed series on constant updates 
// that simulates constant movement for example or time intervals in a non-time related game (turn based game eg)
// This is where you would make state changes in the game such as decreasing ammo etc
void game_structure::update()
{	
	// This game logic keeps the world simulator running:
	player_update();
	
	// make the game do something now...show game activity that the user will then respond to
	// this generates gameplay
	world_update();
}

// Gets time in milliseconds now
long game_structure::get_tick_now()
{
	return timeGetTime();
}

void game_structure::spare_time(long frameTime)
{
	event_admin->process_all_events();
}

void game_structure::draw(float percent_within_tick)
{
	if(config->use_3d_render_manager)
		D3DRenderManager::GetInstance().update();
	else
		sdl_graphics_manager::get().draw_current_scene(false);
}

bool game_structure::initialize_sdl(int screenWidth, int screenHeight)
{
	if(!sdl_graphics_manager::get().initialize( screenWidth, screenHeight)){
		log_message("Failed to initialize SDL graphics manager");
		return false;
	}
			
    
	return true;
}

void game_structure::unload()
{
	Mix_FreeChunk(config->scratch_fx );
    Mix_FreeChunk(config->high_sound_fx );
    Mix_FreeChunk(config->medium_sound_fx );
    Mix_FreeChunk(config->low_sound_fx );

    config->scratch_fx = nullptr;
    config->high_sound_fx = nullptr;
    config->medium_sound_fx = nullptr;
    config->low_sound_fx = nullptr;
    
    //Free the music
    Mix_FreeMusic( config->music );
    config->music = nullptr;

	TTF_CloseFont(config->font);
	config->font  = nullptr;
		
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
		
}


game_structure::game_structure()
{
	g_pGameWorldData = std::make_shared<game_world_data>();
	init_game_world_data();
}

void game_structure::init_game_world_data() const
{
	g_pGameWorldData->is_game_done = false;
	g_pGameWorldData->is_network_game = false;
	g_pGameWorldData->can_render = true;
} // Load audio game files

bool game_structure::load_media()
{
	// Load Music
    config->music = Mix_LoadMUS(resource_admin->get_resource_by_name("MainTheme.wav")->m_path.c_str());

	// Load Audio Fx
    config->scratch_fx = Mix_LoadWAV(resource_admin->get_resource_by_name("scratch.wav")->m_path.c_str());
	config->high_sound_fx = Mix_LoadWAV(resource_admin->get_resource_by_name("high.wav")->m_path.c_str());
	config->medium_sound_fx = Mix_LoadWAV(resource_admin->get_resource_by_name("medium.wav")->m_path.c_str());
	config->low_sound_fx = Mix_LoadWAV(resource_admin->get_resource_by_name("low.wav")->m_path.c_str());

	//Load Font
	config->font =  TTF_OpenFont( "arial.ttf", 28 );

	string msg;

	auto dynamic_string_func = [&](string &base, const char* c_string) -> string&
	{
		if(strlen(c_string) > base.size() && !base.empty())
			base.resize( base.size() *2);
		base = c_string;
		return base;
	};

	// Error reporting
	
	if(config->music == nullptr)
    {
		log_message(dynamic_string_func(msg,  "Failed to load beat music! SDL_mixer Error: ") + Mix_GetError());
        return false;
    }  
    
    if(config->scratch_fx == nullptr)
    {
    	log_message(dynamic_string_func(msg,  "Failed to load scratch sound effect! SDL_mixer Error:") + Mix_GetError());
        return false;
    }
    
    if(config->high_sound_fx == nullptr)
    {
    	log_message(dynamic_string_func(msg,  "Failed to load high sound effect! SDL_mixer Error:") + Mix_GetError());
        return false;
    }
    
    if(config->medium_sound_fx == nullptr)
    {
    	log_message(dynamic_string_func(msg,  "Failed to load medium sound effect! SDL_mixer Error:") + Mix_GetError());
        return false;
    }
    
    if(config->low_sound_fx == nullptr )
    {
    	log_message(dynamic_string_func(msg,  "Failed to load low sound effect! SDL_mixer Error:") + Mix_GetError());
        return false;
    }

	if(config->font == nullptr)
	{
		log_message(dynamic_string_func(msg,  "Failed to load arial font! TTF_OpenFont Error:") + TTF_GetError());
        return false;
	}

	
	return true;
}

bool log_if_false(bool condition, string message)
{
	if(condition == false)
		log_message(message);
	return condition;
}

// Initialize resource, level manager, and load game audio files
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

void game_structure::init3d_render_manager()
{
	auto& renderManager = D3DRenderManager::GetInstance();
	renderManager.Initialize(GetModuleHandle(NULL), 800, 600, false, "My Window");
	auto mesh = new Mesh3D();
	mesh->create();
	D3DRenderManager::GetInstance().meshes.push_back(mesh);
}

shared_ptr<player> game_structure::create_player()
{
	return make_shared<player>(player(config->player_init_pos_x, config->player_init_pos_y,
	                                  config->square_width / 2));
}

void game_structure::add_player_to_scene()
{
	const auto the_player = create_player();	
	the_player->subscribe_to_event(event_type::PositionChangeEventType);	
	the_player->raise_event(std::make_shared<add_game_object_to_current_scene_event>(the_player, 100));
}

bool game_structure::initialize()
{
	return run_and_log("game_structure::initialize()", config->verbose, [&]()
	{
		resource_admin->initialize();

		const auto sdl_ok = log_if_false(initialize_sdl(config->screen_width, config->screen_height), "Could not initialize SDL, aborting.");
		
		if(!sdl_ok)
			return false;
		
		if(config->use_3d_render_manager)
			init3d_render_manager();

		return true;
	});
}

void game_structure::game_loop()
{
	auto tick_count_at_last_call = get_tick_now();
	const auto max_loops = config->max_loops;

	// MAIN GAME LOOP!!
	while (!g_pGameWorldData->is_game_done) 
	{
		const auto new_time =  get_tick_now();
		auto frame_ticks = 0;  // Number of ticks in the update call	
		auto num_loops = 0;  // Number of loops ??
		auto ticks_since = new_time - tick_count_at_last_call;

		// New frame, happens consistently every 50 milliseconds. Ie 20 times a second.
		// 20 times a second = 50 milliseconds
		// 1 second is 20*50 = 1000 milliseconds
		while (ticks_since > config->TICK_TIME_MS && num_loops < max_loops)
		{
			update();		
			tick_count_at_last_call += config->TICK_TIME_MS; // tickCountAtLastCall is now been +Single<GlobalConfig>().TickTime more since the last time. update it
			frame_ticks += config->TICK_TIME_MS; num_loops++;
			ticks_since = new_time - tick_count_at_last_call;
		}

		spare_time(frame_ticks); // handle player input, general housekeeping (Event Manager processing)

		if (g_pGameWorldData->is_network_game || ticks_since <= config->TICK_TIME_MS)
		{
			if (g_pGameWorldData->can_render)
			{
				const auto percent_outside_frame = static_cast<float>(ticks_since / config->TICK_TIME_MS) * 100; // NOLINT(bugprone-integer-division)				
				draw(percent_outside_frame);
			}
		}
		else
		{
			tick_count_at_last_call = new_time - config->TICK_TIME_MS;
		}
	}
	std::cout << "Game done" << std::endl;
}

bool game_structure::load_content()
{
	resource_admin->parse_game_resources();
	
	const auto load_media_ok = log_if_false(load_media(), "Could not load media, aborting.");

	if(!load_media_ok)
		return false;
	
	// Generate the level 
	for (const auto& room: level_generator::generate_level())
	{
		auto game_object = std::dynamic_pointer_cast<square>(room);

		// Add each room to the scene
		game_object->subscribe_to_event(event_type::PlayerMovedEventType);
		game_object->raise_event(std::make_shared<add_game_object_to_current_scene_event>(game_object));
	}

	// Create the player
	add_player_to_scene();

	return true;
}


