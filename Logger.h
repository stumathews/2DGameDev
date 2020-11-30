#pragma once
#include <iostream>
#include "global_config.h"
class logger
{
public:
	static void log_message(const std::string &message, const bool be_verbose = global_config::verbose)
	{
		if (be_verbose) std::cout << message << std::endl;
	}
};
