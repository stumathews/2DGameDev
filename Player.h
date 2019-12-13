#pragma once
#include "Square.h"
class Player : public Square
{
public:
	Player(int x, int y, int w) 
		: Square(x, y, w, true, true){}
	virtual vector<shared_ptr<Event>> ProcessEvent(std::shared_ptr<Event> event);
};

