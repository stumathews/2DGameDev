#pragma once
#include "Component.h"
class player_component : public Component
{
public:
	player_component(string name, int x, int y, int w, int h) : Component(name), x(x), y(y), w(w), h(h)
	{
	}
	int x, y, w, h;
};

