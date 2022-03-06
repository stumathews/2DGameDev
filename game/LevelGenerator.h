#pragma once
#include <vector>
#include <memory>

#include "scene/Room.h"

namespace gamelib
{
	class level_generator
	{
	public:
		enum room_side {top_side = 1, right_side = 2, bottom_side = 3, left_side = 4};
		static std::vector<std::shared_ptr<Room>> generate_level(ResourceManager& resource_admin, SettingsManager& settings_admin, EventManager& event_admin);	
	};
}

