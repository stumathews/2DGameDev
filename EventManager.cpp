#include "EventManager.h"
#include <vector>
#include "EventSubscriber.h"
#include <iostream>


void EventManager::RegisterEvent(std::shared_ptr<Event> evt)
{
	m_events.push_back(evt);
}

void EventManager::SubscribeToEvent(EventType type, IEventSubscriber* you)
{
	m_EventSubscribers[type].push_back(you);	
}

EventManager::~EventManager(){}

void EventManager::ProcessEvents()
{
	// Go through all the registered events and call all ther subscribers telling them about the event
	
	for(auto eventIterator = m_events.begin(); eventIterator != m_events.end();eventIterator++) {
		for(auto subscriber : m_EventSubscribers[(*eventIterator)->m_eventType]) {
			// Call subscriber's ProcessEvent method
			subscriber->ProcessEvent(*eventIterator);			
		}		
	}
	m_events.clear();
}
