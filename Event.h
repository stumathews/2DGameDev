#pragma once

enum event_type { PositionChangeEventType, LevelChangedEventType, DoLogicUpdateEventType, AddGameObjectToCurrentScene, PlayerMovedEventType };

class Event
{
public:
	explicit Event(event_type type) : m_eventType(type), data(nullptr), eventId(-1), processed(false){}
	virtual ~Event();
	virtual void SetData(void* data)
	{
		data = data;
	}
	void* data;
	event_type m_eventType;
	int eventId;
	bool processed;
	
};

