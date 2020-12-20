#pragma once
#include "exceptions/base_exception.h"

class game_exception : public gamelib::base_exception
{
public:
	game_exception(std::string message, std::string subsystem);
};

