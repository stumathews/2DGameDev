#pragma once
#include <vector>
#include <memory>
#include "Room.h"


class LevelGenerator
{
public:	
	LevelGenerator() = delete;
	LevelGenerator(int screenWidth, int screenHeight, int rows, int columns, bool removeRandomSides);
	std::vector<std::shared_ptr<Room>> Generate();	
private:
	int screenWidth, screenHeight, rows, columns;
	bool removeRandomSides;
};


