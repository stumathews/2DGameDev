#pragma once
#include "Event.h"
#include <memory>
#include <vector>
#include <string>

class event_subscriber
{
	public:
	virtual ~event_subscriber();
	virtual std::vector<std::shared_ptr<event>> handle_event(std::shared_ptr<event> evt) = 0;
	virtual std::string get_subscriber_name() = 0;
};

