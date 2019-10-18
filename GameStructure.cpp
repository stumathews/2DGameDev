
#include <SDL.h>
#include <SDL_image.h>
#include <Windows.h>
#include <iostream>
#include "Common.h"
#include "GameObject.h"
#include "Drawing.h"
#include <vector>
#include <SDL.h>
#include <SDL_mixer.h>
#include "Events.h"
#include "ResourceManager.h"
#include "SceneChangedEvent.h"
#include "SceneManager.h"
#include "DoLogicUpdateEvent.h"
#include "RenderManager3D.h"

using namespace std;
//The music that will be played
Mix_Music *gMusic = NULL;

//The sound effects that will be used
Mix_Chunk *gScratch = NULL;
Mix_Chunk *gHigh = NULL;
Mix_Chunk *gMedium = NULL;
Mix_Chunk *gLow = NULL;

bool use3dRengerManager = false;

int main(int argc, char * args[]);

void SetLevel();

void DoGameLoop(long &tickCountAtLastCall, long &newTime, int &frameTicks, int &numLoops);

int Initialize(bool &retflag);

// Create our global copy of the game data
std::shared_ptr<GameWorldData> g_pGameWorldData = std::shared_ptr<GameWorldData>(new GameWorldData());


/***
 * Check for interaction requests from controllers
 */
void sense_player_input()
{
	//Map controller actions to meanings for the game:
	// left button was pushed and button A was pressed MEANS -> request to move character left
	// while shooting active weapon.



	//Event handler
	int interval = 10;
	SDL_Event e;
	while(SDL_PollEvent(&e) != 0)
	{
		if(e.type == SDL_QUIT)
		{
			g_pGameWorldData->bGameDone = 1;
		} else if( e.type == SDL_KEYDOWN ) {
			switch( e.key.keysym.sym )
			{
				case SDLK_UP:
					std::cout << "Player pressed up!" << std::endl;	
					EventManager::GetInstance().RegisterEvent( shared_ptr<PositionChangeEvent>(new PositionChangeEvent(Up)));
				break;

				case SDLK_DOWN:
					std::cout << "Player pressed down!" << std::endl;		
					EventManager::GetInstance().RegisterEvent( shared_ptr<PositionChangeEvent>(new PositionChangeEvent(Down)));
				break;

				case SDLK_LEFT:
					std::cout << "Player pressed left!" << std::endl;					
					EventManager::GetInstance().RegisterEvent( shared_ptr<PositionChangeEvent>(new PositionChangeEvent(Left)));
				break;

				case SDLK_RIGHT:
					std::cout << "Player pressed right!" << std::endl;	
					EventManager::GetInstance().RegisterEvent( shared_ptr<PositionChangeEvent>(new PositionChangeEvent(Right)));
				break;

				case SDLK_q:
					std::cout << "Player pressed Quit key !" << std::endl;
					g_pGameWorldData->bGameDone = 1;
					break;
				case SDLK_j:
					// simluate a level change
					std::cout << "Level change to 1" << std::endl;
					EventManager::GetInstance().RegisterEvent( shared_ptr<SceneChangedEvent>(new SceneChangedEvent(1)));
					break;
				case SDLK_k:
				// simluate a level change
				std::cout << "Level change to 2" << std::endl;
				EventManager::GetInstance().RegisterEvent( shared_ptr<SceneChangedEvent>(new SceneChangedEvent(2)));
				break;
				case SDLK_l:
				// simluate a level change
				std::cout << "Level change to 3" << std::endl;
				EventManager::GetInstance().RegisterEvent( shared_ptr<SceneChangedEvent>(new SceneChangedEvent(3)));
				break;

				 //Play high sound effect
                case SDLK_1:
                Mix_PlayChannel( -1, gHigh, 0 );
                break;
                            
                //Play medium sound effect
                case SDLK_2:
                Mix_PlayChannel( -1, gMedium, 0 );
                break;
                            
                //Play low sound effect
                case SDLK_3:
                Mix_PlayChannel( -1, gLow, 0 );
                break;
                            
                //Play scratch sound effect
                case SDLK_4:
                Mix_PlayChannel( -1, gScratch, 0 );
                break;
				case SDLK_9:
				//If there is no music playing
				if( Mix_PlayingMusic() == 0 )
				{
					//Play the music
					Mix_PlayMusic( gMusic, -1 );
				}
				//If music is being played
				else
				{
					//If the music is paused
					if( Mix_PausedMusic() == 1 )
					{
						//Resume the music
						Mix_ResumeMusic();
					}
					//If the music is playing
					else
					{
						//Pause the music
						Mix_PauseMusic();
					}
				}
				break;
				case SDLK_0:
                //Stop the music
                Mix_HaltMusic();
                break;


				default:
					SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                         "Unkown control key",
                         "press up, down, left, right or 'q' to quit",
                         NULL);
					std::cout << "Unknown control key" << std::endl;	
				break;
			}
		}
	}
}

/***
 * Collision detection, no key for door etc...
 */
void determine_restrictions()
{
	// check for geometric restrictions via the gameworld data to determine whats around the character physically
	// ie. collision detection

	// check for logical restrictions ie the state the player must be in to perform certain interactions
	// ie do i have a door key to even though i'm trying to open the door in fron of me?

	// use the gameworld data to understand where the player is (level) and what he has around him

	// easy restrictions for tetris: 
	// 1) bricks cannot move outside the screen, 
	// 2) cannot fall beyond ground level
	// 3) cannot continue falling if current brick is directly above any other previously fallen brick
}

/***
 * show player result of their interaction.
 * Map the restrictions to the interactions and generate the right world-level responses
 */
void update_player_state()
{
	// Trigger the moving animation, if :
	// 1. the controller is used in such a way that it MEANS move character left
	// 2. There is no restriction to move left.

	// Update the player's resultant state.

	// Tetris: move/rotate the brick according to players input

	// then: AddDefaultBehavior() for idle

}

/***
 * Keeps and updated snapshot of the player state
 */
void player_update()
{
	// Read from game controller
	sense_player_input();
	
	// First see if we can perform what the payer wants us to do (we might be unable to, next to wall ie. cant move forward)
	determine_restrictions();
	
	// Do what we can to update the players state based on the above and what the player tied to do
	// so move the player's position if he asked to move and there was no obstacle etc.
	update_player_state();

	// If networked game:
	// broadcast players state(position eg) to other players on the network
}

/***
 * Select passive elements
 */
void pre_select_active_zones()
{

}

/***
 * Update elements such as walls, and scenario items. These belong to the game world but dont have any
 * attached behavior
 */
void update_passive_elements()
{
	pre_select_active_zones(); // only select elments that will affect gameplay
}



void logic_sort_according_to_relevance(){}
void execute_control_mechanism(){}
void update_state()
{
	// Ask the event manager to notify event subscribers to update their logic now
	EventManager::GetInstance().RegisterEvent(shared_ptr<DoLogicUpdateEvent>(new DoLogicUpdateEvent()));
}

/***
 * Update simple logical elements such as doors, elevators or moving platforms
 */
void update_logic_based_elements()
{
	//not used yet
	logic_sort_according_to_relevance(); // only select elements that are important to the gameplay
	// not used yet
	execute_control_mechanism();
	update_state();
}



/***
 * update the word state based on all the above having been done/happened/occured
 * If shot, mark enemy as hit
 */
void update_world_data_structure()
{

}

/***
 * Generate behavior rules. The plane will turn, then try to shoot
 */
void decision_engine()
{

}


/***
 * the overall restrictions that apply
 */
void sense_restrictions()
{

}

/***
 * goals and current state must be analysed
 *	goals: shoot down aircraft.
 *	state: amount of ammo, direction of aircraft, state of weaponry
 */
void sense_internal_state_and_goals()
{

}

/***
 * An enemy 10 miles away is not important, nor is a door on another level - filter it out.
 */
void ai_sort_according_to_gamplay_relevance()
{

}

/**
 * Update elements that have intelligence like enemies with distinctive behavior
 */
void update_ai_based_elements()
{
	ai_sort_according_to_gamplay_relevance();
	sense_internal_state_and_goals(); // goal: shoot down the player. internal state of element: position and heading, state of weapons systems and sustained damage
	sense_restrictions(); // avoid collisions with wall if im an enemy ai element
	// we know about our state and the players state now...
	
	// determine what the ai elements will do next (ai decisions may span several seconds or minutes of gameplay, probably refining
	// the strategy/decision on each subsequent cycle here)
	decision_engine();

	// store that the enemy moved, or it ws shot so remove it from world data structure
	update_world_data_structure();
}

/***
 * Update active elements such as decorative flying birds or doors that open and close
 */
void update_active_elements()
{
	update_logic_based_elements(); // doors, elevators, movng platforms, real enemies with a distinctive behavior (simple)
	update_ai_based_elements(); // real enemies with artificial intelligence behavior (more complex)
}

/***
 * Updates, monitors what the world is doing around the player. This is usually what the player reacts to
 */
void world_update()
{
	// not used yet
	update_passive_elements(); //walls and most scenario items - have no attached behavior but play a key role in player restrictions
	update_active_elements(); // flying birds, doors that open and close - must be checked to keep consistent, meaningful experiance

}

// This is basically the update functions which is run x FPS to maintain a timed series on constant updates 
// that simulates constant movement for example or time intervals in a non-time related game (turn based game eg)
// This is where you would make state changes in the game such as decreasing ammo etc
void Update()
{	
	// This game logic keeps the world simulator running:
	player_update();
	
	// make the game do something now...show game activity that the user will then respond to
	// this generates gameplay
	world_update();
}

// Gets time in milliseconds now
long ticks()
{
	return timeGetTime();
}

void SpareTime(long frameTime)
{
	EventManager::GetInstance().ProcessEvents();
}

/***
 * Main graphics pipe line for NPC rendering.
 * Send packed NPC geometry to hardware.
 */
void npc_render_data()
{

}

/***
 * Pack generated NPC geometry data (from the animation step) for this frame into an efficient format
 */
void npc_pack_data()
{

}

/***
 * Produce static geometry data that represents the current snapshot of how the character must look
 * for a given frame.
 *
 * The main animation routines are computed.
 * keyframed to skeletal animations *
 */
void npc_animate()
{

}

/***
 * Select NPCs that are visible as they should be rendered only. Ignore those behind you or on other levels
 */
void npc_select_visible_subset()
{

}

/*
  Render characters (Non player Characters)
  These are animated active elements, usually characters such as enemies
 */
void NPC_Presentation()
{
	npc_select_visible_subset(); //only thoe close to the player or affecting him are to be processed. Visibility check is usually used
	npc_animate(); // keyframed to skeletal animations represent a current snapshot of how the character must look for a given frame
	
				   // some animation methods will require specific rendering algorithms to characters will need to be rendered seperately from passive world geometry
	npc_pack_data();
	npc_render_data();
}

/*
 Send player's geometry to the hardware for processing
 */
void player_render_data()
{

}
/***
 * Pack player's geometry into efficient format
 */
void player_data_pack()
{

}
/***
 * Produce static geometry data that represents the current snapshot of how the player must look
 * for a given frame.
 *
 * The main animation routines are computed.
 * keyframed to skeletal animations
 */
void player_animate()
{

}

/***
 * Render player.
 * The player is always visible.
 * Simplier graphics pipeline to NPC and Passive elements
 * no LOD determination - hero is always drawn in High-resoluton meshes
 */
void Player_Presentation()
{
	player_animate();
	player_data_pack();
	player_render_data();
}


/***
 * Send packed audio data to sound hardware(sound card)
 */
void world_send_audio_data_to_audio_hardware()
{
	
}

/***
 * Pack audio data into efficient format
 */
void world_pack_audio_data()
{

}
/***
 * Select audible sources using typically distance vs volume metric
 */
void world_select_audible_sound_sources()
{

}
/**
 * Store geometry in an efficient format
 */
void world_pack_geometry()
{

}


/***
 * Send packed goemetry to hardware for processing.
 * Eg. OpenGL,Direct3D
 */
void world_render_geometry()
{
	
}

// chop off items outside of the players view
void world_elements_clip()
{

}

// remove hidden objects - such as backward facing surfaces
void world_elements_cull()
{

}
void world_elements_occulude()
{

}

// filter away invisible or irrelevant elements to reduce render overhead. Main graphics pipeline.
void world_select_visible_graphic_elements()
{
	world_elements_clip();
	world_elements_cull();
	world_elements_occulude();
}


//Determine from elements'd chacracteristics (distance etc) the LOD to be used 
void world_select_resolution()
{

}

/***
 * Send audio to sound card
 */
void send_audio_to_hardware()
{

	world_select_audible_sound_sources(); // distance vs volume metric + attenuation calcs to determine whats is audible to player
	world_pack_audio_data();
	world_send_audio_data_to_audio_hardware();
}

/***
 * Send graphics to graphics card
 */
void send_geometry_to_hardware()
{
	/* not used yet */ world_pack_geometry(); // vertexes are packed into memory
	world_render_geometry(); //  send to card via OpenGL or DirectX
}

/***
 * Render the game work visually and sonically
 */
void World_Presentation()
{
	// show just the visible part of the gameworld from the player's perspective
	/* not used yet */ world_select_visible_graphic_elements(); // This will contain the 3d Rendering pipeline!
	/* not used yet */world_select_resolution(); // Choose suitable level of detail

	send_geometry_to_hardware();  //paint it onto the screen
	/* not used yet */send_audio_to_hardware(); 
}





/***
 * Render the game world (Presentation) ie represent changes in the gameworld data
 * @param percentWithinTick
 */
void Draw(float percentWithinTick)
{	
	SDLGraphicsManager::GetInstance().DrawCurrentScene();
	
	// Tick 3d Render manager
	if(use3dRengerManager)
		D3DRenderManager::GetInstance().update();
	// Render the game work visually and sonically
	World_Presentation();
	
	// Render non player characters next (not used yet)
	NPC_Presentation();

	// Render the player (not used yet)
	Player_Presentation();

	
}


/*
* Initialize the Graphics subsystem incl. Main Window
* Initialize the Audio subsystem
*/
bool InitSDL()
{
	// Initialise SDL	
	if(!SDLGraphicsManager::GetInstance().Initialize())
		return false;

	// Initialize SDL_mixer 
    if( Mix_OpenAudio( 44100 /*sound frequency*/, MIX_DEFAULT_FORMAT/*sample format*/, 2 /*hardware channels*/, 2048 /*sample size*/ ) < 0 )
    {
		std::cout << "SDL_mixer could not initialize! SDL_mixer Error: " << (char*)Mix_GetError() << std::endl;
        return false;
    }
	return true;
}

void CleanupResources()
{
	Mix_FreeChunk( gScratch );
    Mix_FreeChunk( gHigh );
    Mix_FreeChunk( gMedium );
    Mix_FreeChunk( gLow );
    gScratch = NULL;
    gHigh = NULL;
    gMedium = NULL;
    gLow = NULL;
    
    //Free the music
    Mix_FreeMusic( gMusic );
    gMusic = NULL;
	
	

	IMG_Quit();
	SDL_Quit();

	
}


bool loadMedia()
{
	//Load music
    gMusic = Mix_LoadMUS( ResourceManager::GetInstance().GetResourceByName("MainTheme.wav")->m_path.c_str() );
    
	//Load sound effects
    gScratch = Mix_LoadWAV( ResourceManager::GetInstance().GetResourceByName("scratch.wav")->m_path.c_str() );
	gHigh = Mix_LoadWAV( ResourceManager::GetInstance().GetResourceByName("high.wav")->m_path.c_str() );
	gMedium = Mix_LoadWAV( ResourceManager::GetInstance().GetResourceByName("medium.wav")->m_path.c_str() );
	gLow = Mix_LoadWAV( ResourceManager::GetInstance().GetResourceByName("low.wav")->m_path.c_str() );

	if( gMusic == NULL )
    {
        printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
        return false;
    }  
    
    if( gScratch == NULL )
    {
        printf( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
        return false;
    }
    
    if( gHigh == NULL )
    {
        printf( "Failed to load high sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
        return false;
    }
    
    if( gMedium == NULL )
    {
        printf( "Failed to load medium sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
        return false;
    }
    
    if( gLow == NULL )
    {
        printf( "Failed to load low sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
        return false;
    }
	return true;
}
