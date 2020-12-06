#pragma once
#include <string>
#include <list>
#include <memory>
#include "objects/game_object.h"

class layer
{
public:
	bool visible = false;
	unsigned int zorder = 0;
	float x = 0;
	float y = 0;
	std::string name;
	std::list<std::shared_ptr<game_object>> game_objects;
};


