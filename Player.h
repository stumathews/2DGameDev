#pragma once
#include "Square.h"
class Player : public square
{
public:
	Player(int x, int y, int w) : square(x, y, w, true, true){}
	vector<shared_ptr<Event>> process_event(std::shared_ptr<Event> event) override;
	void draw(SDL_Renderer* renderer) override;
	static void add_player_to_game();
};

