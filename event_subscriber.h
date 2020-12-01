#pragma once
#include "Event.h"
#include <memory>
#include <vector>
#include <string>

using namespace std;

class event_subscriber
{
	public:
	event_subscriber()=default;
	virtual ~event_subscriber();
	virtual vector<shared_ptr<event>> handle_event(std::shared_ptr<event> evt) = 0;
	virtual string get_subscriber_name() = 0;
};

