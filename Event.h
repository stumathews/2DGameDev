#pragma once

enum event_type { PositionChangeEventType, LevelChangedEventType, DoLogicUpdateEventType, AddGameObjectToCurrentScene, PlayerMovedEventType };

class Event
{
public:
	explicit Event(event_type type) : m_eventType(type), m_evtData(nullptr), eventId(-1), processed(false){}
	virtual ~Event();
	virtual void SetData(void* data)
	{
		m_evtData = data;
	}
	void* m_evtData;
	event_type m_eventType;
	int eventId;
	bool processed;
	
};

