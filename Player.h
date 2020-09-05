#pragma once
#include "Square.h"
class Player : public Square
{
public:
	Player(int x, int y, int w) 
		: Square(x, y, w, true, true){}
	virtual vector<shared_ptr<Event>> process_event(std::shared_ptr<Event> event) override;
	virtual void VDraw(SDL_Renderer* renderer) override;
	static void add_player_to_game();
};

