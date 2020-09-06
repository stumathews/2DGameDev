#pragma once
#include "Event.h"
#include <memory>
#include <vector>
#include <string>

using namespace std;

class event_subscriber  // NOLINT(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
{
protected:
	virtual ~event_subscriber();
public:
	virtual vector<shared_ptr<Event>> process_event(std::shared_ptr<Event> evt) = 0;
	virtual string get_subscriber_name() = 0;
};

