#pragma once
#include <vector>
#include <memory>
#include "Event.h"
#include "EventSubscriber.h"
#include <map>

class event_manager  // NOLINT(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
{
	event_manager() = default;
	~event_manager() = default;;
	std::vector<shared_ptr<Event>> primary_event_queue_;
	std::vector<shared_ptr<Event>> secondary_event_queue_;
	map<event_type, std::vector<IEventSubscriber*>> event_subscribers_;
	
	public:
		static event_manager& get_instance()
		{
			static event_manager instance;
			return instance;
		}
		event_manager(event_manager const&) = delete;
		void operator=(event_manager const&) = delete;
		
		void register_event(const std::shared_ptr<Event> event);
		void subscribe_to_event(event_type type, IEventSubscriber* you);		
		void process_all_events();

};

