#pragma once
#include "Event.h"
#include <memory>
#include <vector>

using namespace std;

class IEventSubscriber
{
public:
	virtual vector<shared_ptr<Event>> ProcessEvent(std::shared_ptr<Event> evt) = 0;
};

