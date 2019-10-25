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
#include "AddGameObjectToCurrentSceneEvent.h"
#include "Room.h"
#include <stack>
#include <time.h>

using namespace std;

// 20 times a second = 50 milliseconds
// 1 second is 20*50 = 1000 milliseconds

#define TICK_TIME 50
#define MAX_LOOPS 4

extern std::shared_ptr<GameWorldData> g_pGameWorldData;
extern bool InitSDL();
extern bool use3dRengerManager;

// Load game audio files
extern bool loadMedia();
extern long ticks();
extern void Update();
extern void Draw(float);
extern void SpareTime(long);
extern void CleanupResources();
void DoGameLoop();
bool Initialize();
void Init3dRenderManager();
void Uninitialize();
void SetLevel(int level);

bool unvisitedCells(vector <Room> mazeGrid) {
  for (Uint32 i = 0; i < mazeGrid.size(); i++) {
    if (!mazeGrid[i].isVisited()) {
      return true;
    }
  }
  return false;
}
#define ROWS 800
#define COLUMNS 600

int checkNeighbours(vector<shared_ptr<Room>> maze, shared_ptr<Room> r) {
  int x = r->getX();
  int y = r->getY();
  vector <shared_ptr<Room>> neighbours;
  if(x > 0 && !maze[(x - 1) * ROWS + y]->isVisited()) {
    neighbours.push_back(maze[(x - 1) * ROWS + y]);
  }
  if( x < ROWS - 1 && !maze[(x + 1) * ROWS + y]->isVisited()) {
    neighbours.push_back(maze[(x + 1) * ROWS + y]);
  }
  if(y < COLUMNS - 1 && !maze[x * ROWS + y + 1]->isVisited()) {
    neighbours.push_back(maze[x * ROWS + y + 1]);
  }
  if(y > 0 && !maze[x * ROWS + y - 1]->isVisited()) {
    neighbours.push_back(maze[x * ROWS + y - 1]);
  }
  if (neighbours.size() < 1) {
    return -1;
  }
  
  int randomIdx = rand() % neighbours.size();
  int nxt = randomIdx;//neighbours[randomIdx]->getY() + neighbours[randomIdx]->getX() * ROWS;
  return nxt;
}

int main(int argc, char *args[])
{	
	// Prepare all sub systems
	if(!Initialize())
		return -1;

	// Trigger the first level by kicking the event manager
	EventManager::GetInstance().RegisterEvent(std::shared_ptr<SceneChangedEvent>(new SceneChangedEvent(1)));

	auto screenWidth=800;
	auto screenHeight=600;	
	auto width = 50;
	auto maxRows = screenWidth/width;
	auto maxColumns = screenHeight/width;
	
	 vector<shared_ptr<Room>> mazeGrid;
	 stack<shared_ptr<Room>> roomStack;

	for(int y = 0; y < maxColumns; y++)
	{
		for(int x = 0; x < maxRows; x++)
		{
			auto gameObject = shared_ptr<Room>(new Room(x*width, y*width, width));			
			mazeGrid.push_back(gameObject);			
		}
	}
	auto totalRooms = mazeGrid.size();
	
	srand(time(0));
	for(int i = 0; i < totalRooms; i++)
	{
		auto current = mazeGrid[i];
		auto nextIndex = i + 1;
		auto prevIndex = i - 1;
		if(nextIndex >= totalRooms)
			break;
		auto next = mazeGrid[nextIndex];
		auto row = abs(i / maxColumns);		
		auto lastCol = (row+1 * maxColumns)-1;
		auto col = maxColumns - (lastCol-i);

		bool withinRowBound = row >= 0 && i <= maxRows;
		bool withinColBound = i >= 0 && i <= maxColumns-1;
		
		int roomAbove = i - maxColumns;
		int roomBelow = i + maxColumns;
		int roomLeft = i - 1;
		int roomRight = i + 1;

		bool canRemoveAbove = roomAbove >= 0;
		bool canRemoveBelow = roomBelow < totalRooms; 
		bool canRemoveLeft = col-1 >= 1;
		bool canRemoveRight = col+1 <= maxColumns;

		vector<int> list;
		if(canRemoveAbove && current->IsWalled(1) && mazeGrid[roomAbove]->IsWalled(3))
			list.push_back(1);
		if(canRemoveBelow  && current->IsWalled(3) && mazeGrid[roomBelow]->IsWalled(1))
			list.push_back(3);
		if(canRemoveLeft  && current->IsWalled(4) && mazeGrid[roomLeft]->IsWalled(2))
			list.push_back(4);
		if(canRemoveRight  && current->IsWalled(2) && mazeGrid[roomRight]->IsWalled(4))
			list.push_back(2);

		if(list.size() == 0)
			int i = 0;

		int randIndex = rand() % list.size();
		if(list[randIndex] == 1)
		{
				current->removeWall(1);
				next->removeWall(3);
				continue;
			
		}
		if(list[randIndex] == 2)
		{ 
				current->removeWall(2);
				next->removeWall(4);
				continue;
			
		}
		if(list[randIndex] == 3)
		{
		
				current->removeWall(3);
				next->removeWall(1);
				continue;
			
		}
		if(list[randIndex] == 4)
		{		
			
				current->removeWall(4);				
				auto prev = mazeGrid[prevIndex];
				prev->removeWall(2);
				continue;
		
		}
		
	}
	
	for(auto gameObject : mazeGrid)
	{
			std::shared_ptr<GameObject> cpe = std::dynamic_pointer_cast<Room>(gameObject);
			auto event = std::shared_ptr<AddGameObjectToCurrentSceneEvent>(new AddGameObjectToCurrentSceneEvent(&cpe));
			EventManager::GetInstance().RegisterEvent(event);
	}
	

	    
    
	
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

/* Initialize resource, level manager, and load game audio files
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

	if(use3dRengerManager)
		Init3dRenderManager();	
		
	return true;
}

void Init3dRenderManager()
{
	D3DRenderManager& renderManager = D3DRenderManager::GetInstance();
	renderManager.Initialize(GetModuleHandle(NULL), 800, 600, false, "My Window");
	Mesh3D* mesh = new Mesh3D();
	mesh->create();
	D3DRenderManager::GetInstance().meshes.push_back(mesh);
}
