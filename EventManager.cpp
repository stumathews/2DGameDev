#include "EventManager.h"
#include <vector>
#include "EventSubscriber.h"


void event_manager::raise_event(const shared_ptr<Event> event)  // NOLINT(performance-unnecessary-value-param)
{
	primary_event_queue_.push_back(event);
}

void event_manager::subscribe_to_event(event_type type, IEventSubscriber* you)
{
	event_subscribers_[type].push_back(you);	
}

void event_manager::process_all_events()
{
	auto event_count = 0;
	secondary_event_queue_.clear();
	
	for (const auto &event : primary_event_queue_)
	{
		if(event->processed) // for safety sake
			continue;

		event->eventId = event_count++;

		for (const auto& subscriber :  event_subscribers_[event->m_eventType])
		{
			for(const auto &secondary_event : subscriber->process_event(event))
			{
				secondary_event_queue_.push_back(secondary_event);
			}
		}
		event->processed = true;
	}
	
	primary_event_queue_.clear();

	if(!secondary_event_queue_.empty())
	{
		for(const auto& event : secondary_event_queue_)
		{
			primary_event_queue_.push_back(event);
		}
						
		process_all_events();
	}
}

