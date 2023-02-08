#pragma once
#include <vector>
#include <memory>
#include "Room.h"

class Rooms
{
public:
	static void ConfigureRooms(int rows, int columns, std::vector<std::shared_ptr<Room>>& rooms);
	static void ConfigureWalls(const std::shared_ptr<Room>& thisRoom, bool& canRemoveWallAbove,
	                           std::vector<std::shared_ptr<Room>>& rooms, std::shared_ptr<Room>& nextRoom,
	                           bool& canRemoveWallRight, bool& canRemoveWallBelow, bool& canRemoveWallLeft,
	                           const int& prevIndex);
	static void RemoveAllWalls(const std::shared_ptr<Room>& thisRoom);
	static gamelib::Coordinate<int> CenterOfRoom(const std::shared_ptr<Room>& room, int yourWidth, int yourHeight);
};
