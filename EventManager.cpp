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
	event_subscribers[type].push_back(you);	
}

EventManager::~EventManager(){}

void EventManager::ProcessEvents()
{
	// go through all the events and call all ther subscribers telling them about the event
	
	for(auto it = events.begin(); it != events.end();it++) {
		std::shared_ptr<Event> evt = *it;
		auto subscribers  = event_subscribers[evt->m_eventType];
		for(auto subscriber : subscribers) {
			subscriber->ProcessEvent(evt);			
		}		
	}
	events.clear();
}
