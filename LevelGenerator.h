#pragma once
#include <time.h>
#include <vector>
#include <memory>
#include "Square.h"
#include <stack>
#include "GlobalConfig.h"
#include "Single.h"

class LevelGenerator
{
public:
	enum RoomSide {TopSide = 1, RightSide = 2, BottomSide = 3, LeftSide = 4};
	vector<shared_ptr<Square>> GenerateLevel();
	
};

