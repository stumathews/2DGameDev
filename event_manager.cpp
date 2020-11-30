#include "event_manager.h"
#include <vector>
#include "event_subscriber.h"
#include "Logger.h"
#include "Common.h"


event_manager::event_manager()
{
	logger::log_message("event_manager constructed");
}


void event_manager::raise_event(const shared_ptr<event> event, event_subscriber* you)  // NOLINT(performance-unnecessary-value-param)
{
	auto const log = "event_manager: " + you->get_subscriber_name()  + string(" raised to event ") + event->to_str();
	
	if(event->type != event_type::DoLogicUpdateEventType)
		logger::log_message(log);

	primary_event_queue_.push(event);
}

void event_manager::subscribe_to_event(const event_type type, event_subscriber* you)
{
	auto const message = "event_manager: "+you->get_subscriber_name() + string(" subscribed to event ") + type;
	logger::log_message(message);
	
	event_subscribers_[type].push_back(you);	
}


void event_manager::process_all_events()
{
	auto event_count = 0;

	while(!primary_event_queue_.empty())
	{
		const auto& event = primary_event_queue_.front();
		
		
		if(event->processed) // for safety sake
			continue;

		event->event_id = event_count++;

		// get subscribers of this event type
		for (const auto& subscriber :  event_subscribers_[event->type])
		{
			// allow subscriber to process the event
			for(const auto &secondary_event : subscriber->process_event(event))
			{
				// any results from processing are put onto the secondary queue
				secondary_event_queue_.push(secondary_event);
			}			
		}
		event->processed = true;
		primary_event_queue_.pop();
	}

	// Process the secondary queue once primary queue is processed
	while(!secondary_event_queue_.empty())
	{
		// put the secondary queue onto the back of the the primary queue for next cycle of processing
		primary_event_queue_.push(secondary_event_queue_.front());
		secondary_event_queue_.pop();
	}
}

