#pragma once
#include <map>

enum EventType { PositionChangeEventType, LevelChangedEventType, DoLogicUpdateEventType, AddGameObjectToCurrentScene, PlayerMovedEventType };


class Event
{
public:
	Event(EventType type) : m_eventType(type), m_evtData(NULL), eventId(-1), processed(false){}
	~Event();
	virtual void SetData(void* data)
	{
		m_evtData = data;
	}
	void* m_evtData;
	EventType m_eventType;
	int eventId;
	bool processed;
	
};

