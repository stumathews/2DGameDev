#pragma once
#include "Component.h"
class PlayerComponent : public Component
{
public:
	PlayerComponent(string name, int x, int y, int w, int h) : Component(name), x(x), y(y), w(w), h(h)
	{
	}
	int x, y, w, h;
};

