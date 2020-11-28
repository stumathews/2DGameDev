#include "Event.h"
#include "Common.h"

event::event(event_type type): type(type)
{
	// using member init only
}

event_type event::get_type()
{
	return type;
}

std::string operator+(const std::string& str, const event_type type)
{
	return str + std::to_string(as_integer(type));
}
