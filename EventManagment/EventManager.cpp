#include "EventManager.h"
#include <vector>
#include "EventSubscriber.h"
#include <iostream>


EventManager::EventManager()
{
}

void EventManager::RegisterEvent(std::shared_ptr<Event> evt)
{
	events.push_back(evt);
}

void EventManager::SubscribeToEvent(EventType type, EventSubscriber* you)
{
	std::cout << "Subscribed to event" << std::endl;
	event_subscribers[type].push_back(you);	
}

EventManager::~EventManager(){}

void EventManager::ProcessEvents()
{
	// go through all the events and call all ther subscribers telling them about the event
	
	for(std::vector<std::shared_ptr<Event>>::iterator it = events.begin(); it != events.end();it++)
	{
		std::shared_ptr<Event> evt = *it;
		std::vector<EventSubscriber*> subscribers  = event_subscribers[evt->m_eventType];
		for(EventSubscriber* subscriber : subscribers)
		{
			subscriber->ProcessEvent(evt);			
		}		
	}
	events.clear();
}
