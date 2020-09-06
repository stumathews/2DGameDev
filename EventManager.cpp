#include "EventManager.h"
#include <vector>
#include "EventSubscriber.h"
#include "Logger.h"


event_manager::event_manager()
{
	logger::log_message("event_manager constructed");
}

void event_manager::raise_event(const shared_ptr<Event> event, event_subscriber* you)  // NOLINT(performance-unnecessary-value-param)
{
	if(event->m_eventType != DoLogicUpdateEventType)
		logger::log_message("event_manager:" + you->get_subscriber_name()  + string(" raised to event #") + event->m_eventType);
	primary_event_queue_.push(event);
}

void event_manager::subscribe_to_event(const event_type type, event_subscriber* you)
{
	logger::log_message("event_manager:"+you->get_subscriber_name() + string(" subscribed to event #") + type);
	event_subscribers_[type].push_back(you);	
}

void event_manager::process_all_events()
{
	auto event_count = 0;

	while(!primary_event_queue_.empty())
	{
		const auto event = primary_event_queue_.front();
		primary_event_queue_.pop();
		
		if(event->processed) // for safety sake
			continue;

		event->eventId = event_count++;
				
		for (const auto& subscriber :  event_subscribers_[event->m_eventType])
		{
			for(const auto &secondary_event : subscriber->process_event(event))
			{
				secondary_event_queue_.push(secondary_event);
			}
		}
		event->processed = true;
	}

	while(!secondary_event_queue_.empty())
	{
		primary_event_queue_.push(secondary_event_queue_.front());
		secondary_event_queue_.pop();
	}
}

