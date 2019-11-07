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
#include "Square.h"
#include <stack>
#include <time.h>
#include "PlayerComponent.h"
#include "Player.h"
#include "PlayerMovedEvent.h"

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

bool unvisitedCells(vector <Square> mazeGrid) {
  for (Uint32 i = 0; i < mazeGrid.size(); i++) {
    if (!mazeGrid[i].isVisited()) {
      return true;
    }
  }
  return false;
}
#define ROWS 800
#define COLUMNS 600

int checkNeighbours(vector<shared_ptr<Square>> maze, shared_ptr<Square> r) {
  int m_xPos = r->GetX();
  int m_yPos = r->GetY();
  vector <shared_ptr<Square>> neighbours;
  if(m_xPos > 0 && !maze[(m_xPos - 1) * ROWS + m_yPos]->isVisited()) {
    neighbours.push_back(maze[(m_xPos - 1) * ROWS + m_yPos]);
  }
  if( m_xPos < ROWS - 1 && !maze[(m_xPos + 1) * ROWS + m_yPos]->isVisited()) {
    neighbours.push_back(maze[(m_xPos + 1) * ROWS + m_yPos]);
  }
  if(m_yPos < COLUMNS - 1 && !maze[m_xPos * ROWS + m_yPos + 1]->isVisited()) {
    neighbours.push_back(maze[m_xPos * ROWS + m_yPos + 1]);
  }
  if(m_yPos > 0 && !maze[m_xPos * ROWS + m_yPos - 1]->isVisited()) {
    neighbours.push_back(maze[m_xPos * ROWS + m_yPos - 1]);
  }
  if (neighbours.size() < 1) {
    return -1;
  }
  
  int randomIdx = rand() % neighbours.size();
  int nxt = randomIdx;//neighbours[randomIdx]->getY() + neighbours[randomIdx]->getX() * ROWS;
  return nxt;
}

enum RoomSide {TopSide = 1, RightSide = 2, BottomSide = 3, LeftSide = 4};

int main(int argc, char *args[])
{	
	// Prepare all sub systems
	if(!Initialize())
		return -1;

	// Trigger the first level by kicking the event manager
	EventManager::GetInstance().RegisterEvent(std::shared_ptr<SceneChangedEvent>(new SceneChangedEvent(1)));

	srand(time(0));

	auto screenWidth = 800;
	auto screenHeight = 600;	
	auto roomWidth = 25;
	auto maxRows = screenWidth/roomWidth;
	auto maxColumns = screenHeight/roomWidth;
	
	vector<shared_ptr<Square>> mazeGrid;
	stack<shared_ptr<Square>> roomStack;

	/* Generate Rooms for the Maze */
	for(int y = 0; y < maxColumns; y++)
	{
		for(int x = 0; x < maxRows; x++)
		{
			auto gameObject = shared_ptr<Square>(new Square(x * roomWidth, y * roomWidth, roomWidth, false, false));			
			mazeGrid.push_back(gameObject);			
		}
	}

	auto totalRooms = mazeGrid.size();	
	
	/* Determine which faces/edges can be removed, based on the bounds of the grid i.e within rows x cols of board */
	for(int i = 0; i < totalRooms; i++)
	{		
		auto nextIndex = i + 1;
		auto prevIndex = i - 1;

		if(nextIndex >= totalRooms)
			break;
		
		auto thisRow = abs(i / maxColumns);		
		auto lastColumn = (thisRow+1 * maxColumns)-1;
		auto thisColumn = maxColumns - (lastColumn-i);
			
		int roomAboveIndex = i - maxColumns;
		int roomBelowIndex = i + maxColumns;
		int roomLeftIndex = i - 1;
		int roomRightIndex = i + 1;

		bool canRemoveAbove = roomAboveIndex >= 0;
		bool canRemoveBelow = roomBelowIndex < totalRooms; 
		bool canRemoveLeft = thisColumn-1 >= 1;
		bool canRemoveRight = thisColumn+1 <= maxColumns;

		vector<int> removableSides;
		auto currentRoom = mazeGrid[i];
		auto nextRoom = mazeGrid[nextIndex];

		if(canRemoveAbove && currentRoom->IsWalled(TopSide) && mazeGrid[roomAboveIndex]->IsWalled(BottomSide))
			removableSides.push_back(TopSide);
		if(canRemoveBelow  && currentRoom->IsWalled(BottomSide) && mazeGrid[roomBelowIndex]->IsWalled(TopSide))
			removableSides.push_back(BottomSide);
		if(canRemoveLeft  && currentRoom->IsWalled(LeftSide) && mazeGrid[roomLeftIndex]->IsWalled(RightSide))
			removableSides.push_back(LeftSide);
		if(canRemoveRight  && currentRoom->IsWalled(RightSide) && mazeGrid[roomRightIndex]->IsWalled(LeftSide))
			removableSides.push_back(RightSide);
				
		int randSideIndex = rand() % removableSides.size(); // Choose a random element wall to remove from possible choices
		auto removeSidesRandonly = false;
		if(removeSidesRandonly)
		{
			switch(removableSides[randSideIndex])
			{
			case TopSide:
				currentRoom->removeWall(TopSide);
				nextRoom->removeWall(BottomSide);
				continue;
			case RightSide:
				currentRoom->removeWall(RightSide);
				nextRoom->removeWall(LeftSide);
				continue;
			case BottomSide:
				currentRoom->removeWall(BottomSide);
				nextRoom->removeWall(TopSide);
				continue;
			case LeftSide:
				currentRoom->removeWall(LeftSide);				
				auto prev = mazeGrid[prevIndex];
				prev->removeWall(RightSide);
				continue;
			}
		}
	}
	
	/* Schedule adding rooms to screen */
	for(auto object : mazeGrid)
	{
		std::shared_ptr<GameObject> gameObject = std::dynamic_pointer_cast<Square>(object);		
		gameObject->SubScribeToEvent(PlayerMovedEventType);
		gameObject->RaiseEvent(std::shared_ptr<AddGameObjectToCurrentSceneEvent>(new AddGameObjectToCurrentSceneEvent(&gameObject)));		
	}
	
	/* Schedule adding the player to the screen */
	auto playerWidth = roomWidth / 5;	
	auto playerDetails = shared_ptr<PlayerComponent>(new PlayerComponent("PlayerDetails", 0, 0, playerWidth, playerWidth));
	std::shared_ptr<GameObject> player = std::shared_ptr<GameObject>(new Player(playerDetails->x, playerDetails->y, playerDetails->w));	
	
	player->AddComponent(shared_ptr<Component>(playerDetails));
	player->RaiseEvent(std::shared_ptr<AddGameObjectToCurrentSceneEvent>(new AddGameObjectToCurrentSceneEvent(&player)));
	player->SubScribeToEvent(PositionChangeEventType);

	/* Trigger the player's first move event at 0,0 */
	
	auto playerInitialMoveEvent = shared_ptr<Event>(new PlayerMovedEvent(playerDetails));	
	EventManager::GetInstance().RegisterEvent(playerInitialMoveEvent);
	

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
