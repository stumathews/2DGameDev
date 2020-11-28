#pragma once

#include <SDL.h>
#include <vector>
#include <memory>
#include "event_manager.h"
#include "game_object.h"

#include <iostream>

inline bool failed(bool result)
{
	return result == false;
}

struct game_world_data 
{
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;
	
	bool is_game_done;
	bool is_network_game;
	bool can_render;
		
	// List of game objects
	std::vector<std::shared_ptr<game_object>> actors;
};

