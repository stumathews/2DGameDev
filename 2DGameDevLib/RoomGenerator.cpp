#include "pch.h"
#include "RoomGenerator.h"
#include <algorithm>
#include <random>
#include <vector>
#include <util/SettingsManager.h>
#include "Rooms.h"

using namespace std;
using namespace gamelib;

RoomGenerator::RoomGenerator(const int screenWidth, const int screenHeight, const int rows, const int columns, const bool removeRandomSides)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	this->rows = rows;
	this->columns = columns;
	this->removeRandomSides = removeRandomSides;
}

/// <summary>
/// Generates the Rooms in the Level
/// </summary>
vector<shared_ptr<Room>> RoomGenerator::Generate() const
{
	
	// calculate the dimensions of a room (a square with 4 sides/walls)
	const auto squareWidth = screenWidth / columns; 
	const auto squareHeight = screenHeight / rows;
		
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
			const auto number = count++;
			auto roomName = string("Room") + std::to_string(number);
							
			// Create room
			auto room = std::make_shared<Room>(roomName, "Room", number, col * squareWidth, row * squareHeight,
			                                   squareWidth, squareHeight, false);
				
			// Set room tag to room number
			room->SetTag(std::to_string(number));

			// Add to list of rooms in game
			rooms.push_back(room);			
		}
	}

	ConfigureRooms(rooms);

	return rooms;
}

void RoomGenerator::ConfigureRooms(const std::vector<std::shared_ptr<Room>>& rooms) const
{
	const auto totalRooms = static_cast<int>(rooms.size());

	// Setup room
	for (auto i = 0; i < totalRooms; i++)
	{
		const auto nextIndex = i + 1;
		const auto prevIndex = i - 1;
		const auto roomRow = static_cast<int>(ceil(i / columns));
		const auto roomCol = i % columns;

		// Prevent removing outer walls 
		auto canRemoveTopWall = roomRow >= 1 && roomRow <= (rows - 1);
		auto canRemoveBottomWall = roomRow >= 0 && roomRow < (rows - 1);
		auto canRemoveLeftWall = roomCol > 0 && roomCol <= (columns - 1);
		auto canRemoveRightWall = roomCol >= 0 && roomCol < (columns - 1);

		// Calculate indexes of sorrounding/adjacent rooms
		const auto roomIndexAbove = canRemoveTopWall ? (i - columns) : i;
		const auto roomIndexBelow = canRemoveBottomWall ? (i + columns) : i;
		const auto roomIndexLeft = canRemoveLeftWall ? prevIndex : i;
		const auto roomIndexRight = canRemoveRightWall ? nextIndex : i;
		auto& thisRoom = rooms[i];
		auto& nextRoom = nextIndex == totalRooms ? rooms[i] : rooms[nextIndex];

		thisRoom->SetSorroundingRooms(roomIndexAbove, roomIndexRight, roomIndexBelow, roomIndexLeft);

		ConfigureWalls(thisRoom, canRemoveTopWall, rooms, nextRoom, canRemoveRightWall, canRemoveBottomWall, canRemoveLeftWall, prevIndex);
	}
}

void RoomGenerator::ConfigureWalls(const std::shared_ptr<Room>& thisRoom, const bool& canRemoveWallAbove, const std::vector<std::shared_ptr<Room>>& rooms, const std::shared_ptr<Room>& nextRoom, const bool& canRemoveWallRight, const bool& canRemoveWallBelow, const bool& canRemoveWallLeft, const int& prevIndex) const
{
	if (SettingsManager::Get()->GetBool("grid", "nowalls"))
	{
		// Temporary measure to lift all wall restrictions
		Rooms::RemoveAllWalls(thisRoom);
	}
	else
	{
		RemoveSidesRandomly(canRemoveWallAbove, thisRoom, rooms, nextRoom, canRemoveWallRight, canRemoveWallBelow, canRemoveWallLeft, prevIndex);
	}
}


void RoomGenerator::RemoveSidesRandomly(const bool& canRemoveTopWall, const std::shared_ptr<Room>& currentRoom,
                                        const std::vector<std::shared_ptr<Room>>& rooms, const std::shared_ptr<Room>& nextRoom,
                                        const bool& canRemoveRightWall, const bool& canRemoveBottomWall,
                                        const bool& canRemoveLeftWall, const int& prevIndex) const
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
	vector<Side> randomSides;
	constexpr uint nSample = 1;
	std::sample(begin(removableSides), end(removableSides), std::back_inserter(randomSides), nSample, std::mt19937{ std::random_device{}() });

	// Removes one side randomly
	if (removeRandomSides)
	{
		const auto aSide = randomSides.front();

		if (aSide == Side::Top && canRemoveTopWall)
		{
			// remove the top side of this room
			currentRoom->RemoveWallZeroBased(Side::Top);

			const auto& roomAbove = rooms[currentRoom->GetNeighbourIndex(Side::Top)];

			// remove the bottom of the room above 
			roomAbove->RemoveWallZeroBased(Side::Bottom);

			nextRoom->RemoveWallZeroBased(Side::Bottom);
		}

		if (aSide == Side::Right && canRemoveRightWall)
		{
			// Remove the right side of this room
			currentRoom->RemoveWallZeroBased(Side::Right);
			const auto& roomToLeft = rooms[currentRoom->GetNeighbourIndex(Side::Right)];

			// Remove room on right's left wall
			roomToLeft->RemoveWallZeroBased(Side::Left);

			nextRoom->RemoveWallZeroBased(Side::Left);
		}

		if (aSide == Side::Bottom && canRemoveBottomWall)
		{
			// Remove the bottom side of this room
			currentRoom->RemoveWallZeroBased(Side::Bottom);

			// Remove the room below top wall
			const auto& roomBelow = rooms[currentRoom->GetNeighbourIndex(Side::Bottom)];
			roomBelow->RemoveWallZeroBased(Side::Top);

			nextRoom->RemoveWallZeroBased(Side::Top);
		}

		if (aSide == Side::Left && canRemoveLeftWall)
		{
			// Remove the left wall on this room
			currentRoom->RemoveWallZeroBased(Side::Left);
			const auto& prev = rooms[prevIndex];

			// Remove the room on left side's right wall
			const auto& roomOnLeft = rooms[currentRoom->GetNeighbourIndex(Side::Left)];
			roomOnLeft->RemoveWallZeroBased(Side::Right);

			prev->RemoveWallZeroBased(Side::Right);
		}
	}
}
