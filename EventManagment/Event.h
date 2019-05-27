#pragma once
#include <map>
enum EventType { PositionChangeEventType, LevelChangedEventType, DoLogicUpdateEventType };

class Event
{
public:
	Event(EventType type) : m_eventType(type){};
	~Event();
	virtual void SetData(void* data)
	{
		m_evtData = data;
	}
	void* m_evtData;
	EventType m_eventType;
	
	
};

