#pragma once
#include <string>

class base_exception : public std::exception
{
public:
	base_exception(std::string message, std::string subsystem);
	std::string subsystem;
	virtual std::string get_source_subsystem() const {return subsystem;}
};

