#pragma once
#include "Events.h"

class scene_changed_event final : public Event
{
public:
	scene_changed_event(int level) : Event(LevelChangedEventType)
	{
		Event::SetData(reinterpret_cast<int*>(level));
		m_Level = level;
	}
	~scene_changed_event();
	int m_Level;

};


