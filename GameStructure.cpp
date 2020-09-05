
#include <SDL.h>
#include <SDL_image.h>
#include <Windows.h>
#include <iostream>
#include "Common.h"
#include "Single.h"
#include <SDL_mixer.h>
#include "Events.h"
#include "ResourceManager.h"
#include "SceneChangedEvent.h"
#include "SceneManager.h"
#include "DoLogicUpdateEvent.h"
#include "RenderManager3D.h"
#include <SDL_ttf.h>
#include "GlobalConfig.h"
#include "GameStructure.h"
#include <functional>
using namespace std;

int main(int argc, char * args[]);

// Create our global copy of the game data
std::shared_ptr<GameWorldData> g_pGameWorldData = std::make_shared<GameWorldData>();


void log_message(const string &message, const bool be_verbose = Single<GlobalConfig>().verbose)
{
	if (be_verbose) std::cout << message << std::endl;
}

void GameStructure::get_input() const
{
	const auto be_verbose = Singleton<GlobalConfig>::GetInstance().object.verbose;	
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
					event_manager::get_instance().raise_event(std::make_unique<PositionChangeEvent>(Up));
				break;
				case SDLK_s:
				case SDLK_DOWN:
					log_message("Player pressed down!", be_verbose);	
					event_manager::get_instance().raise_event(std::make_unique<PositionChangeEvent>(Down));
				break;
				case SDLK_a:
				case SDLK_LEFT:
					log_message("Player pressed left!", be_verbose);					
					event_manager::get_instance().raise_event(std::make_unique<PositionChangeEvent>(Left));
				break;

				case SDLK_d:
				case SDLK_RIGHT:
					log_message("Player pressed right!", be_verbose);	
					event_manager::get_instance().raise_event(std::make_unique<PositionChangeEvent>(Right));
				break;

				case SDLK_q:
					log_message("Player pressed quit!", be_verbose);
					g_pGameWorldData->bGameDone = 1;
					break;
				case SDLK_j:
					log_message("Change to level 2", be_verbose);
					event_manager::get_instance().raise_event(std::make_unique<scene_changed_event>(1));
					break;
				case SDLK_k:
					log_message("Change to level 2", be_verbose);
					event_manager::get_instance().raise_event(std::make_unique<scene_changed_event>(2));
				break;
				case SDLK_l:
					log_message("Change to level 3", be_verbose);
					event_manager::get_instance().raise_event(std::make_unique<scene_changed_event>(3));
				break;

				case SDLK_x:
					log_message("Change to level 4", be_verbose);
					event_manager::get_instance().raise_event(std::make_unique<scene_changed_event>(4));
				break;	
                case SDLK_1:
					Mix_PlayChannel( -1, Singleton<GlobalConfig>::GetInstance().object.gHigh, 0 );
                break;                            
                case SDLK_2:
					Mix_PlayChannel( -1, Singleton<GlobalConfig>::GetInstance().object.gMedium, 0 );
                break;
                case SDLK_3:
					Mix_PlayChannel( -1, Singleton<GlobalConfig>::GetInstance().object.gLow, 0 );
                break;
                case SDLK_4:
					Mix_PlayChannel( -1, Singleton<GlobalConfig>::GetInstance().object.gScratch, 0 );
                break;
				case SDLK_9:
					if( Mix_PlayingMusic() == 0 ) {
						Mix_PlayMusic( Singleton<GlobalConfig>::GetInstance().object.gMusic, -1 );
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
void GameStructure::player_update() const
{
	// Read from game controller
	get_input();	
}




void GameStructure::update_state()
{
	// Ask the event manager to notify event subscribers to update their logic now
	event_manager::get_instance().raise_event(make_unique<do_logic_update_event>());
}

/***
 * Update simple logical elements such as doors, elevators or moving platforms
 */
void GameStructure::update_logic_based_elements()
{	
	update_state();
}



/***
 * Update active elements such as decorative flying birds or doors that open and close
 */
void GameStructure::update_active_elements()
{
	update_logic_based_elements(); // doors, elevators, movng platforms, real enemies with a distinctive behavior (simple)
}

/***
 * Updates, monitors what the world is doing around the player. This is usually what the player reacts to
 */
void GameStructure::world_update()
{
	update_active_elements(); // flying birds, doors that open and close - must be checked to keep consistent, meaningful experiance

}

// This is basically the update functions which is run x FPS to maintain a timed series on constant updates 
// that simulates constant movement for example or time intervals in a non-time related game (turn based game eg)
// This is where you would make state changes in the game such as decreasing ammo etc
void GameStructure::update()
{	
	// This game logic keeps the world simulator running:
	player_update();
	
	// make the game do something now...show game activity that the user will then respond to
	// this generates gameplay
	world_update();
}

// Gets time in milliseconds now
long GameStructure::get_tick_now()
{
	return timeGetTime();
}

void GameStructure::spare_time(long frameTime)
{
	event_manager::get_instance().process_all_events();
}

void GameStructure::draw(float percentWithinTick)
{
	const auto use_3d_renderer = Singleton<GlobalConfig>::GetInstance().object.use3dRengerManager;
	
	SDLGraphicsManager::GetInstance().draw_current_scene(false);

	if(use_3d_renderer)
		D3DRenderManager::GetInstance().update();
}

bool GameStructure::init_sdl(int screenWidth, int screenHeight)
{
	typedef unique_ptr<string> string_ptr;
	
	if(!SDLGraphicsManager::GetInstance().Initialize( screenWidth, screenHeight)){
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

void GameStructure::cleanup_resources()
{
	Mix_FreeChunk( Singleton<GlobalConfig>::GetInstance().object.gScratch );
    Mix_FreeChunk( Singleton<GlobalConfig>::GetInstance().object.gHigh );
    Mix_FreeChunk( Singleton<GlobalConfig>::GetInstance().object.gMedium );
    Mix_FreeChunk( Singleton<GlobalConfig>::GetInstance().object.gLow );

    Singleton<GlobalConfig>::GetInstance().object.gScratch = nullptr;
    Singleton<GlobalConfig>::GetInstance().object.gHigh = nullptr;
    Singleton<GlobalConfig>::GetInstance().object.gMedium = nullptr;
    Singleton<GlobalConfig>::GetInstance().object.gLow = nullptr;
    
    //Free the music
    Mix_FreeMusic( Singleton<GlobalConfig>::GetInstance().object.gMusic );
    Singleton<GlobalConfig>::GetInstance().object.gMusic = nullptr;

	TTF_CloseFont(Singleton<GlobalConfig>::GetInstance().object.font);
	Singleton<GlobalConfig>::GetInstance().object.font  = nullptr;
	
	
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();


	
}


// Load audio game files
bool GameStructure::load_media()
{
    Singleton<GlobalConfig>::GetInstance().object.gMusic = Mix_LoadMUS( ResourceManager::GetInstance().GetResourceByName("MainTheme.wav")->m_path.c_str() );
    Singleton<GlobalConfig>::GetInstance().object.gScratch = Mix_LoadWAV( ResourceManager::GetInstance().GetResourceByName("scratch.wav")->m_path.c_str() );
	Singleton<GlobalConfig>::GetInstance().object.gHigh = Mix_LoadWAV( ResourceManager::GetInstance().GetResourceByName("high.wav")->m_path.c_str() );
	Singleton<GlobalConfig>::GetInstance().object.gMedium = Mix_LoadWAV( ResourceManager::GetInstance().GetResourceByName("medium.wav")->m_path.c_str() );
	Singleton<GlobalConfig>::GetInstance().object.gLow = Mix_LoadWAV( ResourceManager::GetInstance().GetResourceByName("low.wav")->m_path.c_str() );
	//Singleton<GlobalConfig>::GetInstance().object.font = TTF_OpenFont("arial.ttf", 25);

	string msg;

	auto dynamic_string = [&](string &base, const char* c_string) -> string&
	{
		if(strlen(c_string) > base.size() && !base.empty())
			base.resize( base.size() *2);
		base = c_string;
		return base;
	};
	
	if(Singleton<GlobalConfig>::GetInstance().object.gMusic == nullptr)
    {
		log_message(dynamic_string(msg,  "Failed to load beat music! SDL_mixer Error: ") + Mix_GetError());
        return false;
    }  
    
    if(Singleton<GlobalConfig>::GetInstance().object.gScratch == nullptr)
    {
    	log_message(dynamic_string(msg,  "Failed to load scratch sound effect! SDL_mixer Error:") + Mix_GetError());
        return false;
    }
    
    if(Singleton<GlobalConfig>::GetInstance().object.gHigh == nullptr)
    {
    	log_message(dynamic_string(msg,  "Failed to load high sound effect! SDL_mixer Error:") + Mix_GetError());
        return false;
    }
    
    if(Singleton<GlobalConfig>::GetInstance().object.gMedium == nullptr)
    {
    	log_message(dynamic_string(msg,  "Failed to load medium sound effect! SDL_mixer Error:") + Mix_GetError());
        return false;
    }
    
    if(Singleton<GlobalConfig>::GetInstance().object.gLow == nullptr )
    {
    	log_message(dynamic_string(msg,  "Failed to load low sound effect! SDL_mixer Error:") + Mix_GetError());
        return false;
    }

	
	return true;
}

/* Initialize resource, level manager, and load game audio files
*
*/
bool GameStructure::initialize(int screen_width, int screen_height)
{
	const auto use_3d_renderer = Singleton<GlobalConfig>::GetInstance().object.use3dRengerManager;

	ResourceManager::GetInstance().Initialize();	
	CurrentLevelManager::GetInstance().Initialize();
		
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
	
	if(use_3d_renderer)
		init3d_render_manager();	
		
	return true;
}

void GameStructure::init3d_render_manager()
{
	D3DRenderManager& renderManager = D3DRenderManager::GetInstance();
	renderManager.Initialize(GetModuleHandle(NULL), 800, 600, false, "My Window");
	Mesh3D* mesh = new Mesh3D();
	mesh->create();
	D3DRenderManager::GetInstance().meshes.push_back(mesh);
}


