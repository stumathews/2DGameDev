#pragma once
#include <string>
#include <list>
#include <memory>
#include "game_object.h"
class layer
{
public:
	bool visible = false;
	unsigned int zorder = 0;
	float x = 0;
	float y = 0;
	std::string name = {};
	std::list<shared_ptr<game_object>> game_objects = {};
};


