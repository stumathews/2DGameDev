#pragma once
#include "Events.h"

class scene_changed_event final : public event
{
public:
	explicit scene_changed_event(int level) : event(event_type::LevelChangedEventType)
	{
		scene_id = level;
	}
	int scene_id;
	std::string to_str() override;
};

