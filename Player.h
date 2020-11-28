#pragma once
#include "Square.h"
class player : public square
{
	void add_player_to_scene() const;
public:
	player(int x, int y, int w);
	vector<shared_ptr<event>> process_event(std::shared_ptr<event> event) override;
	void draw(SDL_Renderer* renderer) override;
	
};

