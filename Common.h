#pragma once

#include <SDL.h>
#include "Ball.h"
#include <vector>

struct GameWorldData {
	int x;
	int y;
	int w;
	int h;
	SDL_Window* window;
	SDL_Renderer* windowRenderer;
	SDL_Surface* windowImageSurface;
	bool bGameDone;
	bool bNetworkGame;
	bool bCanRender;	
	std::vector<Actor*> actors;	
};

