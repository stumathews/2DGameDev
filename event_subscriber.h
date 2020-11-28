#pragma once
#include "Event.h"
#include <memory>
#include <vector>
#include <string>

using namespace std;

class event_subscriber  // NOLINT(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
{
	public:
	event_subscriber()=default;
	virtual ~event_subscriber();
	virtual vector<shared_ptr<event>> process_event(std::shared_ptr<event> evt) = 0;
	virtual string get_subscriber_name() = 0;
};

