#include "pch.h"
#include "LevelGenerator.h"
#include <algorithm>
#include <iostream>
#include <random>
#include <vector>
#include <util/SettingsManager.h>
#include <stack>

using namespace std;
using namespace gamelib;

LevelGenerator::LevelGenerator(int screenWidth, int screenHeight, int rows, int columns, bool removeRandomSides)
	: screenWidth(screenWidth), screenHeight(screenHeight), rows(rows), columns(columns), removeRandomSides(removeRandomSides)
{

}

/// <summary>
/// Generates the Rooms in the Level
/// </summary>
vector<shared_ptr<Room>> LevelGenerator::Generate()
{
	
	// caclulate the dimensions of a room (a square with 4 sides/walls)
	const auto square_width = screenWidth / columns; 
	const auto square_height = screenHeight / rows;
		
	// List of Rooms generated
	vector<shared_ptr<Room>> rooms;

	// Generate Row x Col Rooms for the Maze
	auto count = 0;

	// Generate n rows
	for(auto row = 0; row < rows; row++)
	{
		// generate n columns in this row
		for(auto col = 0; col < columns; col++)
		{			
			// each room has a unique room number
			auto number = count++;
							
			// Create room
			auto room = std::shared_ptr<Room>(new Room(number, col * square_width, row * square_height, square_width, square_height, false));				
				
			// Set room tag to room number
			room->SetTag(std::to_string(number));

			// Add to list of rooms in game
			rooms.push_back(room);			
		}
	}

	const auto totalRooms = static_cast<int>(rooms.size());	
		
	// Setup room
	for(auto i = 0; i < totalRooms; i++)
	{
		const auto nextIndex = i + 1;
		const auto prevIndex = i - 1;
		const auto roomRow = (int)ceil(i/columns);
		const auto roomCol = i % columns;		

		// Prevent removing outer walls 
		auto canRemoveTopWall = roomRow >= 1 && roomRow <= (rows-1);
		auto canRemoveBottomWall = roomRow >= 0 && roomRow < (rows-1);
		auto canRemoveLeftWall = roomCol > 0 && roomCol <= (columns-1);
		auto canRemoveRightWall = roomCol >= 0 && roomCol < (columns-1);

		// Calculate indexes of sorrounding/adjacent rooms
		auto roomIndexAbove = canRemoveTopWall ? (i - columns) : i;
		auto roomIndexBelow = canRemoveBottomWall ? (i + columns) : i;
		auto roomIndexLeft = canRemoveLeftWall ? prevIndex : i;
		auto roomIndexRight = canRemoveRightWall ? nextIndex : i;
		auto &thisRoom = rooms[i];
		auto &nextRoom = nextIndex == totalRooms ? rooms[i] : rooms[nextIndex];

		thisRoom->SetSoroundingRooms(roomIndexAbove, roomIndexRight, roomIndexBelow, roomIndexLeft);

		ConfigureWalls(thisRoom, canRemoveTopWall, rooms, nextRoom, canRemoveRightWall, canRemoveBottomWall, canRemoveLeftWall, prevIndex);
	}

	return rooms;
}

void LevelGenerator::ConfigureWalls(std::shared_ptr<Room>& thisRoom, bool& canRemoveWallAbove, std::vector<std::shared_ptr<Room>>& rooms, std::shared_ptr<Room>& nextRoom, bool& canRemoveWallRight, bool& canRemoveWallBelow, bool& canRemoveWallLeft, const int& prevIndex)
{
	if (SettingsManager::Get()->GetBool("grid", "nowalls"))
	{
		// Temporary measure to lift all wall restrictions
		RemoveAllWalls(thisRoom);
	}
	else
	{
		RemoveSidesRandomly(canRemoveWallAbove, thisRoom, rooms, nextRoom, canRemoveWallRight, canRemoveWallBelow, canRemoveWallLeft, prevIndex);
	}
}

void LevelGenerator::RemoveAllWalls(std::shared_ptr<Room>& thisRoom)
{
	thisRoom->RemoveWall(Side::Top);
	thisRoom->RemoveWall(Side::Right);
	thisRoom->RemoveWall(Side::Bottom);
	thisRoom->RemoveWall(Side::Left);
}

void LevelGenerator::RemoveSidesRandomly(const bool& canRemoveTopWall, std::shared_ptr<Room>& current_room, std::vector<std::shared_ptr<Room>>& rooms, std::shared_ptr<Room>& next_room, const bool& canRemoveRightWall, const bool& canRemoveBottomWall, const bool& canRemoveLeftWall, const int& prevIndex)
{
	// List of sides that are removable (some cannot be removed if they are on the bounds of the board/game)
	vector<Side> removableSides;

	// is possible to remove top wall of this room?
	if (canRemoveTopWall)
	{
		removableSides.push_back(Side::Top);
	}

	// is possible to remove bottom wall of this room?
	if (canRemoveBottomWall)
	{
		removableSides.push_back(Side::Bottom);
	}

	// is possible to remove left wall of this room?
	if (canRemoveLeftWall)
	{
		removableSides.push_back(Side::Left);
	}

	// is possible to remove right wall of this room?
	if (canRemoveRightWall)
	{
		removableSides.push_back(Side::Right);
	}

	// Choose a 1 random element wall to remove from possible sides 
	vector<Side> random_sides;
	uint n_sample = 1;
	std::sample(begin(removableSides), end(removableSides), std::back_inserter(random_sides), n_sample, std::mt19937{ std::random_device{}() });

	const auto remove_sides_randomly = removeRandomSides;

	// Removes one side randonly
	if (remove_sides_randomly)
	{
		auto aSide = random_sides.front();

		if (aSide == Side::Top && canRemoveTopWall)
		{
			// remove the top side of this room
			current_room->RemoveWallZeroBased(Side::Top);

			auto roomAbove = rooms[current_room->GetNeighbourIndex(Side::Top)];

			// remove the bottom of the room above 
			roomAbove->RemoveWallZeroBased(Side::Bottom);

			next_room->RemoveWallZeroBased(Side::Bottom);
		}

		if (aSide == Side::Right && canRemoveRightWall)
		{
			// Remove the right side of this room
			current_room->RemoveWallZeroBased(Side::Right);
			auto roomToLeft = rooms[current_room->GetNeighbourIndex(Side::Right)];

			// Remove room on right's left wall
			roomToLeft->RemoveWallZeroBased(Side::Left);

			next_room->RemoveWallZeroBased(Side::Left);
		}

		if (aSide == Side::Bottom && canRemoveBottomWall)
		{
			// Remove the bottom side of this room
			current_room->RemoveWallZeroBased(Side::Bottom);

			// Remove the room belows top wall
			auto roomBelow = rooms[current_room->GetNeighbourIndex(Side::Bottom)];
			roomBelow->RemoveWallZeroBased(Side::Top);

			next_room->RemoveWallZeroBased(Side::Top);
		}

		if (aSide == Side::Left && canRemoveLeftWall)
		{
			// Remove the left wall on this room
			current_room->RemoveWallZeroBased(Side::Left);
			auto& prev = rooms[prevIndex];

			// Remove the room on left side's right wall
			auto roomOnLeft = rooms[current_room->GetNeighbourIndex(Side::Left)];
			roomOnLeft->RemoveWallZeroBased(Side::Right);

			prev->RemoveWallZeroBased(Side::Right);
		}
	}
}
