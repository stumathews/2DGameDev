#pragma once
#include <vector>
#include <memory>
#include "Room.h"

class RoomGenerator
{
public:	
	RoomGenerator() = delete;
	RoomGenerator(int screenWidth, int screenHeight, int rows, int columns, bool removeRandomSides);
	std::vector<std::shared_ptr<Room>> Generate();
	void ConfigureRooms(std::vector<std::shared_ptr<Room>>& rooms);
	void ConfigureWalls(std::shared_ptr<Room>& thisRoom, bool& canRemoveWallAbove, std::vector<std::shared_ptr<Room>>& rooms, std::shared_ptr<Room>& nextRoom, bool& canRemoveWallRight, bool& canRemoveWallBelow, bool& canRemoveWallLeft, const int& prevIndex);
	void RemoveSidesRandomly(const bool& can_remove_above, std::shared_ptr<Room>& current_room, std::vector<std::shared_ptr<Room>>& rooms, std::shared_ptr<Room>& next_room, const bool& can_remove_right, const bool& can_remove_below, const bool& can_remove_left, const int& prevIndex);

private:
	int screenWidth, screenHeight, rows, columns;
	bool removeRandomSides;
};


