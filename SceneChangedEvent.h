#pragma once
#include "Events.h"
class SceneChangedEvent : public Event
{
public:
	SceneChangedEvent(int level) : Event(LevelChangedEventType)
	{
		SetData((void*)level);
		m_Level = level;
	}
	~SceneChangedEvent();
	int m_Level;

};


