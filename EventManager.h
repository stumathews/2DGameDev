#pragma once
#include <vector>
#include <memory>
#include "Event.h"
#include "EventSubscriber.h"
class EventManager
{
public:
	EventManager();
	void RegisterEvent(std::shared_ptr<Event> evt);
	void SubscribeToEvent(EventType type, EventSubscriber* you);
	~EventManager();
	void ProcessEvents();
private:	
	std::vector<std::shared_ptr<Event>> events;
	std::map<EventType, std::vector<EventSubscriber*>> event_subscribers;

};

