#pragma once
#include <vector>
#include <memory>
#include "event_subscriber.h"
#include <map>
#include <queue>

#include "Event.h"

class event_manager  // NOLINT(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
{
	std::queue<std::shared_ptr<event>> primary_event_queue_;
	
	// used to hold events occuring out of processing of primary events
	std::queue<std::shared_ptr<event>> secondary_event_queue_;

	// Event subscribers (aka event handlers)
	std::map<event_type, std::vector<event_subscriber*>> event_subscribers_;
	
	public:
	event_manager();
	~event_manager() = default;

	// Cannot copy an event manager
	event_manager(event_manager const&) = delete;
	
	// Cannot assign to an event manager
	void operator=(event_manager const&) = delete;

	bool initialize();
	
	// Raise an arbitrary event
	void raise_event(const std::shared_ptr<event> event, event_subscriber* you);

	// Add yourself as an event handler (aka event subscriber) to handle an event of type event_type
	void subscribe_to_event(event_type type, event_subscriber* you);

	// Goes through all the events on the primary and secondary queues and passes them to their respective event handlers
	// Primary queue is incremented with event when an event is raised via the raise_event() function. This can be one at any time.
	// secondary queue is composed automatically of events that occured while processing the primary queue. 
	void process_all_events();
};
