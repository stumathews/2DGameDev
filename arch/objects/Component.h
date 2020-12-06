#pragma once
#include <string>

class component
{
public:
	explicit component(const std::string name);
	std::string name;
	std::string get_name() const;
	void set_name(std::string name);
};

