#pragma once
#include "Events.h"

class scene_changed_event final : public event
{
public:
	explicit scene_changed_event(int level) : event(LevelChangedEventType)
	{
		scene = level;
	}
	int scene;
};

