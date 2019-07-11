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
	// Go through all the registered events and call all ther subscribers telling them about the event
	
	for(auto eventIterator = events.begin(); eventIterator != events.end();eventIterator++) {
		for(auto subscriber : event_subscribers[(*eventIterator)->m_eventType]) {
			// Call subscriber's ProcessEvent method
			subscriber->ProcessEvent(*eventIterator);			
		}		
	}
	events.clear();
}
