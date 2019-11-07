#pragma once
#include "Square.h"
class Player : public Square
{
public:
	Player(int x, int y, int w) : Square(x, y, w, true, true){}
	virtual void ProcessEvent(std::shared_ptr<Event> event);
	virtual void VDraw(SDL_Renderer* renderer) override;
	virtual void VDoLogic() override;
};

