#pragma once
#include <SDL.h>

class Drawing
{
public:
	Drawing();
	static void DrawRectangle(const int x, const int y, const int w, const int h, SDL_Renderer *onRenderer, int r, int g, int b);
	
	~Drawing();
};

