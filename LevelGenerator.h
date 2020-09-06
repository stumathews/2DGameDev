#pragma once
#include <vector>
#include <memory>
#include "Square.h"

class level_generator
{
public:
	enum room_side {top_side = 1, right_side = 2, bottom_side = 3, left_side = 4};
	static vector<shared_ptr<square>> generate_level();	
};

