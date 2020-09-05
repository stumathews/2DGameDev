#pragma once
#include "Event.h"
class do_logic_update_event : public Event
{
public:
	do_logic_update_event() : Event(DoLogicUpdateEventType)
	{
		Event::SetData(nullptr);
	};
	~do_logic_update_event();
};

