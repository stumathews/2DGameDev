#include "LevelGenerator.h"

vector<shared_ptr<Square>> LevelGenerator::GenerateLevel()
{
	srand(time(0));

	auto screenWidth = Single<GlobalConfig>().ScreenWidth;
	auto screenHeight = Single<GlobalConfig>().ScreenHeight;	
	auto squareWidth = Single<GlobalConfig>().squareWidth;
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
	return mazeGrid;
}
