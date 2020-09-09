#pragma once
#include <vector>
#include <memory>
#include "Event.h"
#include "EventSubscriber.h"
#include <map>
#include <queue>

class event_manager  // NOLINT(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
{
	
	queue<shared_ptr<Event>> primary_event_queue_;
	queue<shared_ptr<Event>> secondary_event_queue_;
	map<event_type, std::vector<event_subscriber*>> event_subscribers_;
	
	public:
	event_manager();
	~event_manager() = default;
		event_manager(event_manager const&) = delete;
		void operator=(event_manager const&) = delete;
		
		void raise_event(const std::shared_ptr<Event> event, event_subscriber* you);
		void subscribe_to_event(event_type type, event_subscriber* you);		
		void process_all_events();

};

