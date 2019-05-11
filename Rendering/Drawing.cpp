#include "Drawing.h"



Drawing::Drawing()
{
}

void Drawing::DrawRectangle(const int x, const int y, const int w, const int h, SDL_Renderer * onRenderer, int r, int g, int b)
{

	// Define a rectangle
	SDL_Rect fillRect = { x == 0 ? 3 : x , y == 0 ? 4 : y , w, h };
	
	// Set draw colour on renderer
	SDL_SetRenderDrawColor(onRenderer, r, g, b, 0xFF);

	// Send to senderer
	SDL_RenderFillRect(onRenderer, &fillRect);
}


Drawing::~Drawing()
{
}
