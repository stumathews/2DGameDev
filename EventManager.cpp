#include "EventManager.h"
#include <vector>
#include "EventSubscriber.h"
#include <iostream>


void event_manager::register_event(std::shared_ptr<Event> evt)
{
	primary_event_queue_.push_back(evt);
}

void event_manager::subscribe_to_event(event_type type, IEventSubscriber* you)
{
	event_subscribers_[type].push_back(you);	
}


// Go through all the registered events and call all their subscribers telling them about the event
void event_manager::process_all_events()
{
	auto event_count = 0;
	secondary_event_queue_.clear();
	
	for (const auto &event : primary_event_queue_)
	{
		if(event->processed) // for safety sake
			continue;

		event->eventId = event_count++;

		// Allow each subscriber to process the event
		auto& subscribers = event_subscribers_[event->m_eventType];
		for (const auto& subscriber : subscribers)
		{	
			// Process events for subscriber
			auto newGeneratedEvents = subscriber->ProcessEvent(event);
			for(const auto &newEvent : newGeneratedEvents)
			{
				secondary_event_queue_.push_back(newEvent);
			}
		}
		event->processed = true;
	}

	event_count = 0;
	primary_event_queue_.clear();

	// Process any events tat were generated during the last processEvents()
	if(secondary_event_queue_.size() > 0)
	{
		// transfer new events to main eventQueue and process again
		for(auto event : secondary_event_queue_)
		{
			auto eventType = event->m_eventType;
			primary_event_queue_.push_back(event);

		}
				
		process_all_events(); // Process again
	}
}
