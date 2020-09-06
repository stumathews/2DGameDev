#include "Event.h"



Event::~Event()
{
}

std::string operator+(const std::string& str, const event_type type)
{
	return str + std::to_string(type);
}
