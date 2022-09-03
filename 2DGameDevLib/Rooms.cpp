#include "pch.h"
#include "Rooms.h"
#include <util/SettingsManager.h>

using namespace gamelib;

void Rooms::ConfigureRooms(int rows, int columns, std::vector<std::shared_ptr<Room>>& rooms)
{
	const auto totalRooms = static_cast<int>(rooms.size());

	// Setup room
	for (auto i = 0; i < totalRooms; i++)
	{
		const auto nextIndex = i + 1;
		const auto prevIndex = i - 1;
		const auto roomRow = (int)ceil(i / columns);
		const auto roomCol = i % columns;

		// Prevent removing outer walls 
		auto canRemoveTopWall = roomRow >= 1 && roomRow <= (rows - 1);
		auto canRemoveBottomWall = roomRow >= 0 && roomRow < (rows - 1);
		auto canRemoveLeftWall = roomCol > 0 && roomCol <= (columns - 1);
		auto canRemoveRightWall = roomCol >= 0 && roomCol < (columns - 1);

		// Calculate indexes of sorrounding/adjacent rooms
		auto roomIndexAbove = canRemoveTopWall ? (i - columns) : i;
		auto roomIndexBelow = canRemoveBottomWall ? (i + columns) : i;
		auto roomIndexLeft = canRemoveLeftWall ? prevIndex : i;
		auto roomIndexRight = canRemoveRightWall ? nextIndex : i;
		auto& thisRoom = rooms[i];
		auto& nextRoom = nextIndex == totalRooms ? rooms[i] : rooms[nextIndex];

		thisRoom->SetSorroundingRooms(roomIndexAbove, roomIndexRight, roomIndexBelow, roomIndexLeft);

		ConfigureWalls(thisRoom, canRemoveTopWall, rooms, nextRoom, canRemoveRightWall, canRemoveBottomWall, canRemoveLeftWall, prevIndex);
	}
}

void Rooms::ConfigureWalls(std::shared_ptr<Room>&thisRoom, bool& canRemoveWallAbove, std::vector<std::shared_ptr<Room>>&rooms, std::shared_ptr<Room>&nextRoom, bool& canRemoveWallRight, bool& canRemoveWallBelow, bool& canRemoveWallLeft, const int& prevIndex)
{
	if (SettingsManager::Get()->GetBool("grid", "nowalls"))
	{
		// Temporary measure to lift all wall restrictions
		RemoveAllWalls(thisRoom);
	}
}

void Rooms::RemoveAllWalls(std::shared_ptr<Room>&thisRoom)
{
	thisRoom->RemoveWall(Side::Top);
	thisRoom->RemoveWall(Side::Right);
	thisRoom->RemoveWall(Side::Bottom);
	thisRoom->RemoveWall(Side::Left);
}
