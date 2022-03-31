#pragma once
#include <vector>
#include <memory>

#include "Room.h"


class level_generator
{
public:
	static std::vector<std::shared_ptr<Room>> generate_level();	
};


