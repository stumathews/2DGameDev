#pragma once
#include "Event.h"

class do_logic_update_event final : public event
{
public:
	do_logic_update_event();
	std::string to_str() override;
};

