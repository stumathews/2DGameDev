#pragma once
#include "Events.h"

class scene_changed_event : public event
{
public:
	explicit scene_changed_event(int level) : event(event_type::LevelChangedEventType)
	{
		scene = level;
	}
	int scene;
	string to_str() override;
};

