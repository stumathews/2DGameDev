#pragma once
#include "../arch/exceptions/base_exception.h"
class game_exception : public base_exception
{
public:
	game_exception(std::string message, std::string subsystem);
};

