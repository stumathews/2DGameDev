#include "EventManager.h"
#include <vector>
#include "EventSubscriber.h"
#include <iostream>


void EventManager::RegisterEvent(std::shared_ptr<Event> evt)
{
	m_primaryEventQ.push_back(evt);
}

void EventManager::SubscribeToEvent(EventType type, IEventSubscriber* you)
{
	m_EventSubscribers[type].push_back(you);	
}


// Go through all the registered events and call all ther subscribers telling them about the event
void EventManager::ProcessEvents()
{	

	int eventCount = 0;
	secondaryEventQ.clear();
	for(int eventIndex = 0; eventIndex < m_primaryEventQ.size(); eventIndex++)
	{
		auto event = m_primaryEventQ.at(eventIndex);
		
		if(event->processed) // for safety sake
			continue;

		event->eventId = eventCount++;

		// Allow each subscriber to process the event
		auto& subscribers = m_EventSubscribers[event->m_eventType];
		for(int subscriberIndex = 0; subscriberIndex < subscribers.size(); subscriberIndex++)
		{	
			// Process events for subscriber
			auto newGeneratedEvents = subscribers[subscriberIndex]->ProcessEvent(event);
			for(auto newEvent : newGeneratedEvents)
			{
				auto eventType = newEvent->m_eventType;
				secondaryEventQ.push_back(newEvent);
			}
		}
		event->processed = true;
	}

	eventCount = 0;
	m_primaryEventQ.clear();

	// Process any events tat were generated during the last processEvents()
	if(secondaryEventQ.size() > 0)
	{
		// transfer new events to main eventQueue and process again
		for(auto event : secondaryEventQ)
		{
			auto eventType = event->m_eventType;
			m_primaryEventQ.push_back(event);

		}
				
		ProcessEvents(); // Process again
	}
}
