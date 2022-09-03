#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Room.h"

class Level
{
public:
	Level(std::string filename);
	void Load();
	std::vector<std::shared_ptr<Room>> Rooms;
	std::string FileName;
	int NumCols;
	int NumRows;
};