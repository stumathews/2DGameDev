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
#include "constants.h"
#include <SDL_ttf.h>
#include "GlobalConfig.h"
#include "GameStructure.h"
#include "Single.h"

using namespace std;

typedef Singleton<GameStructure> MyGameStructure;

// 20 times a second = 50 milliseconds
// 1 second is 20*50 = 1000 milliseconds

const int TickTime = 50;
const int MaxLoops = 4;

bool Initialize(int w, int h);
void Init3dRenderManager();
void Uninitialize();
void DoGameLoop();
const int ScreenWidth = 800;
const int ScreenHeight = 600;


enum RoomSide {TopSide = 1, RightSide = 2, BottomSide = 3, LeftSide = 4};

int main(int argc, char *args[])
{	
	// Prepare all sub systems
	if(!Initialize(ScreenWidth, ScreenHeight))
		return -1;

	bool setInitialLevel = false;

	// Trigger the first level by kicking the event manager
	EventManager::GetInstance().RegisterEvent(std::shared_ptr<SceneChangedEvent>(new SceneChangedEvent(1)));

	srand(time(0));

	auto screenWidth = ScreenWidth;
	auto screenHeight = ScreenHeight;	
	auto squareWidth = 30;
	auto maxRows = screenWidth/squareWidth;
	auto maxColumns = screenHeight/squareWidth;
	
	vector<shared_ptr<Square>> mazeGrid;
	stack<shared_ptr<Square>> roomStack;

	/* Generate Rooms for the Maze */
	int count = 0;
	for(int y = 0; y < maxColumns; y++)
	{
		for(int x = 0; x < maxRows; x++)
		{
			auto supportMoveLogic = false;
			auto gameObject = shared_ptr<Square>(new Square(x * squareWidth, y * squareWidth, squareWidth, supportMoveLogic));				
			gameObject->SetTag(std::to_string(count++));
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
		auto removeSidesRandonly = true;
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
	
	for(auto object : mazeGrid)
	{
		std::shared_ptr<GameObject> gameObject = std::dynamic_pointer_cast<Square>(object);		
		gameObject->SubScribeToEvent(PlayerMovedEventType);
		gameObject->RaiseEvent(std::shared_ptr<AddGameObjectToCurrentSceneEvent>(new AddGameObjectToCurrentSceneEvent(&gameObject)));		
	}
	
	/* Schedule adding the player to the screen */
	auto playerWidth = squareWidth;
	auto playerPosX = 0;
	auto playerPosY = 0;
	auto playerComponent = shared_ptr<PlayerComponent>(new PlayerComponent(constants::playerComponentName, playerPosX, playerPosY, playerWidth, playerWidth));
	auto playerObject = std::shared_ptr<GameObject>(new Player(playerComponent->x, playerComponent->y, playerComponent->w));	

	playerObject->SetTag(constants::playerTag);	
	playerObject->AddComponent(shared_ptr<Component>(playerComponent));
	playerObject->SubScribeToEvent(PositionChangeEventType);
	
	auto addToSceneEvent = std::shared_ptr<AddGameObjectToCurrentSceneEvent>(new AddGameObjectToCurrentSceneEvent(&playerObject));
	addToSceneEvent->eventId = 100;	
	playerObject->RaiseEvent(addToSceneEvent);

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
	Single<GameStructure>().CleanupResources();
}



/* Main game loop
* Separates Rendering, event processing and logic updates
*/
void DoGameLoop()
{
	int frameTicks; // Number of ticks in the update call	
	int numLoops; // Number of loops ??
	long tickCountAtLastCall, newTime;
	tickCountAtLastCall = Single<GameStructure>().ticks();

	// MAIN GAME LOOP!!
	while (!Single<GameStructure>().g_pGameWorldData->bGameDone) {
		newTime = Single<GameStructure>().ticks();
		frameTicks = 0;
		numLoops = 0;
		long ticksSince = newTime - tickCountAtLastCall;

		// New frame, happens consistently every 50 milliseconds. Ie 20 times a second.
		// 20 times a second = 50 milliseconds
		// 1 second is 20*50 = 1000 milliseconds
		while ((ticksSince) > TickTime && numLoops < MaxLoops)
		{
			Single<GameStructure>().Update(); // logic/update			
			tickCountAtLastCall += TickTime; // tickCountAtLastCall is now been +TickTime more since the last time. update it
			frameTicks += TickTime; numLoops++;
			ticksSince = newTime - tickCountAtLastCall;
		}

		Single<GameStructure>().SpareTime(frameTicks); // handle player input, general housekeeping (Event Manager processing)

		if (!Single<GameStructure>().g_pGameWorldData->bNetworkGame && (ticksSince > TickTime)) {
			tickCountAtLastCall = newTime - TickTime;
		}
		else if (Single<GameStructure>().g_pGameWorldData->bCanRender) {
			auto percentOutsideFrame = (float)(ticksSince / TickTime) * 100;
			Single<GameStructure>().Draw(percentOutsideFrame);
		}
	}
	std::cout << "Game done" << std::endl;
}

/* Initialize resource, level manager, and load game audio files
*
*/
bool Initialize(int screenWidth, int screenHeight)
{
	ResourceManager::GetInstance().Initialize();	
	CurrentLevelManager::GetInstance().Initialize();
	
	

	//Single<GameStructure>().InitGameWorldData();
	
	if (!Single<GameStructure>().InitSDL(ScreenWidth, ScreenHeight))
	{
		std::cout << "Could not initailize SDL, aborting." << std::endl;
		return false;
	}

	// Load audio game files
	if (!Single<GameStructure>().loadMedia())
		return -1;		
	if(Singleton<GlobalConfig>::GetInstance().object.use3dRengerManager)
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
