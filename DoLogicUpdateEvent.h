#pragma once
#include "Event.h"
class DoLogicUpdateEvent : 	public Event
{
public:
	DoLogicUpdateEvent() : Event(DoLogicUpdateEventType)
	{
		SetData(NULL);
	};
	~DoLogicUpdateEvent();
};

