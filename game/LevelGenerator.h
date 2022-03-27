#pragma once
#include <vector>
#include <memory>

#include "scene/Room.h"

namespace gamelib
{
	class level_generator
	{
	public:
		static std::vector<std::shared_ptr<Room>> generate_level();	
	};
}

