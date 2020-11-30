#pragma once
#include "Event.h"
class do_logic_update_event : public event
{
public:
	do_logic_update_event();
	string to_str() override;
};

