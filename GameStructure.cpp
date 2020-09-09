
#include <SDL.h>
#include <SDL_image.h>
#include <Windows.h>
#include <iostream>
#include "Common.h"
#include <SDL_mixer.h>
#include "Events.h"
#include "ResourceManager.h"
#include "SceneChangedEvent.h"
#include "SceneManager.h"
#include "DoLogicUpdateEvent.h"
#include "RenderManager3D.h"
#include <SDL_ttf.h>
#include "global_config.h"
#include "GameStructure.h"
#include <functional>
#include "Logger.h"
using namespace std;

extern shared_ptr<event_manager> event_admin;
extern shared_ptr<resource_manager> resource_admin;

// Create our global copy of the game data
std::shared_ptr<GameWorldData> g_pGameWorldData = std::make_shared<GameWorldData>();


void log_message(const string &message, const bool be_verbose = global_config::verbose)
{
	logger::log_message(message, be_verbose);
}



void game_structure::get_input()
{
	const auto be_verbose = global_config::verbose;	
	SDL_Event e;
	
	while(SDL_PollEvent(&e) != 0)
	{
		if(e.type == SDL_QUIT) 
		{
			g_pGameWorldData->bGameDone = true;
		}
		else if( e.type == SDL_KEYDOWN ) 
		{
			switch( e.key.keysym.sym )
			{
				case SDLK_w:
				case SDLK_UP:
					log_message("Player pressed up!", be_verbose);
					event_admin->raise_event(std::make_unique<PositionChangeEvent>(Up), this);
				break;
				case SDLK_s:
				case SDLK_DOWN:
					log_message("Player pressed down!", be_verbose);	
					event_admin->raise_event(std::make_unique<PositionChangeEvent>(Down), this);
				break;
				case SDLK_a:
				case SDLK_LEFT:
					log_message("Player pressed left!", be_verbose);					
					event_admin->raise_event(std::make_unique<PositionChangeEvent>(Left), this);
				break;

				case SDLK_d:
				case SDLK_RIGHT:
					log_message("Player pressed right!", be_verbose);	
					event_admin->raise_event(std::make_unique<PositionChangeEvent>(Right), this);
				break;

				case SDLK_q:
					log_message("Player pressed quit!", be_verbose);
					g_pGameWorldData->bGameDone = 1;
					break;
				case SDLK_j:
					log_message("Change to level 2", be_verbose);
					event_admin->raise_event(std::make_unique<scene_changed_event>(1), this);
					break;
				case SDLK_k:
					log_message("Change to level 2", be_verbose);
					event_admin->raise_event(std::make_unique<scene_changed_event>(2), this);
				break;
				case SDLK_l:
					log_message("Change to level 3", be_verbose);
					event_admin->raise_event(std::make_unique<scene_changed_event>(3),this);
				break;

				case SDLK_x:
					log_message("Change to level 4", be_verbose);
					event_admin->raise_event(std::make_unique<scene_changed_event>(4), this);
				break;	
                case SDLK_1:
					Mix_PlayChannel( -1, Singleton<global_config>::GetInstance().object.high_sound_fx, 0 );
                break;                            
                case SDLK_2:
					Mix_PlayChannel( -1, Singleton<global_config>::GetInstance().object.medium_sound_fx, 0 );
                break;
                case SDLK_3:
					Mix_PlayChannel( -1, Singleton<global_config>::GetInstance().object.low_sound_fx, 0 );
                break;
                case SDLK_4:
					Mix_PlayChannel( -1, Singleton<global_config>::GetInstance().object.scratch_fx, 0 );
                break;
				case SDLK_9:
					if( Mix_PlayingMusic() == 0 ) {
						Mix_PlayMusic( Singleton<global_config>::GetInstance().object.music, -1 );
					} else 	{
						if( Mix_PausedMusic() == 1 )
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

vector<shared_ptr<Event>> game_structure::process_event(std::shared_ptr<Event> evt)
{
	return vector<shared_ptr<Event>>();
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

void game_structure::draw(float percentWithinTick)
{
	const auto use_3d_renderer = global_config::use_3d_render_manager;
	
	sdl_graphics_manager::get().draw_current_scene(false);

	if(use_3d_renderer)
		D3DRenderManager::GetInstance().update();
}

bool game_structure::init_sdl(int screenWidth, int screenHeight)
{
	typedef unique_ptr<string> string_ptr;
	
	if(!sdl_graphics_manager::get().initialize( screenWidth, screenHeight)){
		log_message("Failed to initialize SDL graphics manager");
		return false;
	}
	
    if(Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0 )
    {
	    const string message("SDL_mixer could not initialize! SDL_mixer Error: ");
		log_message(message + Mix_GetError());
        return false;
    }

	/*if(!TTF_Init())
	{
		const string message("Could not initialize TTF");
		log_message(message);
		return false;
	}*/

	return true;
}

void game_structure::cleanup_resources()
{
	Mix_FreeChunk( Singleton<global_config>::GetInstance().object.scratch_fx );
    Mix_FreeChunk( Singleton<global_config>::GetInstance().object.high_sound_fx );
    Mix_FreeChunk( Singleton<global_config>::GetInstance().object.medium_sound_fx );
    Mix_FreeChunk( Singleton<global_config>::GetInstance().object.low_sound_fx );

    Singleton<global_config>::GetInstance().object.scratch_fx = nullptr;
    Singleton<global_config>::GetInstance().object.high_sound_fx = nullptr;
    Singleton<global_config>::GetInstance().object.medium_sound_fx = nullptr;
    Singleton<global_config>::GetInstance().object.low_sound_fx = nullptr;
    
    //Free the music
    Mix_FreeMusic( Singleton<global_config>::GetInstance().object.music );
    Singleton<global_config>::GetInstance().object.music = nullptr;

	TTF_CloseFont(Singleton<global_config>::GetInstance().object.font);
	Singleton<global_config>::GetInstance().object.font  = nullptr;
		
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
		
}


game_structure::game_structure()
{
	g_pGameWorldData = std::make_shared<GameWorldData>();
	init_game_world_data();
}

void game_structure::init_game_world_data() const
{
	g_pGameWorldData->bGameDone = false;
	g_pGameWorldData->bNetworkGame = false;
	g_pGameWorldData->bCanRender = true;
} // Load audio game files
bool game_structure::load_media()
{
    Singleton<global_config>::GetInstance().object.music = Mix_LoadMUS( resource_admin->get_resource_by_name("MainTheme.wav")->m_path.c_str() );
    Singleton<global_config>::GetInstance().object.scratch_fx = Mix_LoadWAV( resource_admin->get_resource_by_name("scratch.wav")->m_path.c_str() );
	Singleton<global_config>::GetInstance().object.high_sound_fx = Mix_LoadWAV( resource_admin->get_resource_by_name("high.wav")->m_path.c_str() );
	Singleton<global_config>::GetInstance().object.medium_sound_fx = Mix_LoadWAV( resource_admin->get_resource_by_name("medium.wav")->m_path.c_str() );
	Singleton<global_config>::GetInstance().object.low_sound_fx = Mix_LoadWAV( resource_admin->get_resource_by_name("low.wav")->m_path.c_str() );
	//Singleton<GlobalConfig>::GetInstance().object.font = TTF_OpenFont("arial.ttf", 25);

	string msg;

	auto dynamic_string = [&](string &base, const char* c_string) -> string&
	{
		if(strlen(c_string) > base.size() && !base.empty())
			base.resize( base.size() *2);
		base = c_string;
		return base;
	};
	
	if(Singleton<global_config>::GetInstance().object.music == nullptr)
    {
		log_message(dynamic_string(msg,  "Failed to load beat music! SDL_mixer Error: ") + Mix_GetError());
        return false;
    }  
    
    if(Singleton<global_config>::GetInstance().object.scratch_fx == nullptr)
    {
    	log_message(dynamic_string(msg,  "Failed to load scratch sound effect! SDL_mixer Error:") + Mix_GetError());
        return false;
    }
    
    if(Singleton<global_config>::GetInstance().object.high_sound_fx == nullptr)
    {
    	log_message(dynamic_string(msg,  "Failed to load high sound effect! SDL_mixer Error:") + Mix_GetError());
        return false;
    }
    
    if(Singleton<global_config>::GetInstance().object.medium_sound_fx == nullptr)
    {
    	log_message(dynamic_string(msg,  "Failed to load medium sound effect! SDL_mixer Error:") + Mix_GetError());
        return false;
    }
    
    if(Singleton<global_config>::GetInstance().object.low_sound_fx == nullptr )
    {
    	log_message(dynamic_string(msg,  "Failed to load low sound effect! SDL_mixer Error:") + Mix_GetError());
        return false;
    }

	
	return true;
}

/* Initialize resource, level manager, and load game audio files
*
*/
bool game_structure::initialize(int screen_width, int screen_height)
{
	log_message("game_structure::initialize()");
	
	resource_admin->initialize();	
			
	if (!init_sdl(screen_width, screen_height))
	{
		log_message("Could not initialize SDL, aborting.");
		return false;
	}
		
	if (!load_media())
	{
		log_message("Could not load media, aborting.");
		return false;
	}
	
	if(global_config::use_3d_render_manager)
		init3d_render_manager();	
		
	return true;
}

void game_structure::init3d_render_manager()
{
	auto& renderManager = D3DRenderManager::GetInstance();
	renderManager.Initialize(GetModuleHandle(NULL), 800, 600, false, "My Window");
	auto mesh = new Mesh3D();
	mesh->create();
	D3DRenderManager::GetInstance().meshes.push_back(mesh);
}


