#pragma once
#include <iostream>
#include "GlobalConfig.h"
class logger
{
public:
	static void log_message(const std::string &message, const bool be_verbose = GlobalConfig::verbose)
	{
		if (be_verbose) std::cout << message << std::endl;
	}
};
