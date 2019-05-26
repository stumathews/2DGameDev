#pragma once
#include "Events.h"
class LevelChangedEvent : public Event
{
public:
	LevelChangedEvent(int level) : Event(LevelChangedEventType)
	{
		SetData((void*) level);
		m_Level = level;

	}
		~LevelChangedEvent();
		int m_Level;

};