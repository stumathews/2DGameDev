#pragma once

#include <memory>
#include <vector>
#include "events/Event.h"
#include "scene/Square.h"

class player final : public square
{

public:
	player(int x, int y, int w);
	
	std::vector<std::shared_ptr<event>> handle_event(std::shared_ptr<event> event) override;
	void draw(SDL_Renderer* renderer) override;
	std::string get_identifier() override;
};

