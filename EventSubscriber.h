#pragma once
#include "Event.h"
#include <memory>


class IEventSubscriber
{
public:
	virtual void ProcessEvent(std::shared_ptr<Event> evt) = 0;
};

