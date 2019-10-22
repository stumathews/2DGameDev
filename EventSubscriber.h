#pragma once
#include "Event.h"
#include <memory>


class EventSubscriber
{
public:
	EventSubscriber();
	~EventSubscriber();
	virtual void ProcessEvent(std::shared_ptr<Event> evt) = 0;
};

