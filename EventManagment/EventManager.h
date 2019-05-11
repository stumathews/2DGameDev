#pragma once
#include <vector>
#include <memory>
#include "Event.h"
#include "EventSubscriber.h"
class EventManager
{
public:
	static EventManager& getInstance()
	{
		static EventManager instance;
		return instance;
	}
	EventManager(EventManager const&) = delete;
	void operator=(EventManager const&) = delete;
	
	void RegisterEvent(std::shared_ptr<Event> evt);
	void SubscribeToEvent(EventType type, EventSubscriber* you);
	
	void ProcessEvents();
private:	
	EventManager();
	~EventManager();
	std::vector<std::shared_ptr<Event>> events;
	std::map<EventType, std::vector<EventSubscriber*>> event_subscribers;

};

