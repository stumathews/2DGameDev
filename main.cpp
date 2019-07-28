#include <SDL.h>
#include <SDL_image.h>
#include <Windows.h>
#include <iostream>
#include "Common.h"
#include "Ball.h"
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

// 20 times a second = 50 milliseconds
// 1 second is 20*50 = 1000 milliseconds

#define TICK_TIME 50
#define MAX_LOOPS 4

extern std::shared_ptr<GameWorldData> g_pGameWorldData;
extern bool InitSDL();

// Load game audio files
extern bool loadMedia();
extern long ticks();
extern void Update();
extern void Draw(float);
extern void SpareTime(long);
extern void CleanupResources();
void DoGameLoop();
bool Initialize();
void Uninitialize();
void SetLevel(int level);

int main(int argc, char *args[])
{	
	// Prepare all sub systems
	if(!Initialize())
		return -1;

	// Trigger the first level by kicking the event manager
	EventManager::GetInstance().RegisterEvent(std::shared_ptr<SceneChangedEvent>(new SceneChangedEvent(1)));
	
	// Process events, render and update
	DoGameLoop();	
	Uninitialize();	
	return 0;
}


/* Cleans up resources
* Frees surfaces audio files etc
*/
void Uninitialize()
{
	CleanupResources();
}

/* Main game loop
* Separates Rendering, event processing and logic updates
*/
void DoGameLoop()
{
	int frameTicks; // Number of ticks in the update call	
	int numLoops; // Number of loops ??
	long tickCountAtLastCall, newTime;
	tickCountAtLastCall = ticks();

	// MAIN GAME LOOP!!
	while (!g_pGameWorldData->bGameDone) {
		newTime = ticks();
		frameTicks = 0;
		numLoops = 0;
		long ticksSince = newTime - tickCountAtLastCall;

		// New frame, happens consistently every 50 milliseconds. Ie 20 times a second.
		// 20 times a second = 50 milliseconds
		// 1 second is 20*50 = 1000 milliseconds
		while ((ticksSince) > TICK_TIME && numLoops < MAX_LOOPS)
		{
			Update(); // logic/update			
			tickCountAtLastCall += TICK_TIME; // tickCountAtLastCall is now been +TICK_TIME more since the last time. update it
			frameTicks += TICK_TIME; numLoops++;
			ticksSince = newTime - tickCountAtLastCall;
		}

		SpareTime(frameTicks); // handle player input, general housekeeping (Event Manager processing)

		if (!g_pGameWorldData->bNetworkGame && (ticksSince > TICK_TIME)) {
			tickCountAtLastCall = newTime - TICK_TIME;
		}
		else if (g_pGameWorldData->bCanRender) {
			auto percentOutsideFrame = (float)(ticksSince / TICK_TIME) * 100;
			Draw(percentOutsideFrame);
		}
	}
	std::cout << "Game done" << std::endl;
}

/* Initialize resource,level manager, and load game audio files
*
*/
bool Initialize()
{
	ResourceManager::GetInstance().Initialize();	
	CurrentLevelManager::GetInstance().Initialize();
	
	g_pGameWorldData->bGameDone = 0;
	g_pGameWorldData->bNetworkGame = 0;
	g_pGameWorldData->bCanRender = true;
	
		if (!InitSDL())
		{
			std::cout << "Could not initailize SDL, aborting." << std::endl;
			return false;
		}

		// Load audio game files
		if (!loadMedia())
			return -1;	
	
	
		RenderManager3D& renderManager = RenderManager3D::GetInstance();
		renderManager.Initialize(GetModuleHandle(NULL), 800, 600, false, "My Window");
		Mesh3D* mesh = new Mesh3D();
		mesh->create();
		RenderManager3D::GetInstance().meshes.push_back(mesh);
	
		
	return true;
}
