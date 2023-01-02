#include "pch.h"
#include "DrawableFrameRate.h"
#include <util\RectDebugging.h>
#include <SDL.h>

using namespace gamelib;

DrawableFrameRate::DrawableFrameRate(SDL_Rect* bounds)
{
	drawBounds = bounds;
}

void DrawableFrameRate::Update(const float deltaMs) 
{
	if (accumulatedUpdateTime <= 1000)
	{
		accumulatedUpdateTime += deltaMs;
		countUpdates++;
	}
	else
	{
		framesPerSecond = countUpdates;
		accumulatedUpdateTime = countUpdates = 0;
	}
};

void DrawableFrameRate::Draw(SDL_Renderer* renderer) 
{
	RectDebugging::printInRect(renderer, std::to_string(framesPerSecond) + " FPS", drawBounds, {0,0,0, 0});	
}

