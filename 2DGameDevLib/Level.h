#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Room.h"

class Level
{
public:

	Level(std::string filename);
	Level();
	void Load();
	std::vector<std::shared_ptr<Room>> Rooms;
	std::string FileName;
	int NumCols;
	int NumRows;
	unsigned int ScreenWidth;
	unsigned int ScreenHeight;
	bool IsAutoLevel() { return isAutoLevel || FileName.empty(); }
private:
	bool isAutoLevel;
};