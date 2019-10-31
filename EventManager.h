#pragma once
#include <vector>
#include <memory>
#include "Event.h"
#include "EventSubscriber.h"
class EventManager
{
public:
	static EventManager& GetInstance()
	{
		static EventManager instance;
		return instance;
	}
	EventManager(EventManager const&) = delete;
	void operator=(EventManager const&) = delete;
	
	void RegisterEvent(std::shared_ptr<Event> evt);
	void SubscribeToEvent(EventType type, IEventSubscriber* you);
	
	
	/* Go through all the events and call all ther subscribers telling them about the event
	*
	*/
	void ProcessEvents();
private:	
	EventManager(){}
	~EventManager();
	std::vector<std::shared_ptr<Event>> m_events;
	std::map<EventType, std::vector<IEventSubscriber*>> m_EventSubscribers;

};

