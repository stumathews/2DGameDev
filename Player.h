#pragma once
#include "Square.h"
class player : public square
{

public:
	player(int x, int y, int w);
	vector<shared_ptr<event>> handle_event(std::shared_ptr<event> event) override;
	void draw(SDL_Renderer* renderer) override;
	string get_identifier() override;
};

