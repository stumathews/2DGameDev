#include "pch.h"
#include "DrawableFrameRate.h"
#include <util\RectDebugging.h>
#include <SDL.h>
#include <events\Event.h>

using namespace gamelib;

DrawableFrameRate::DrawableFrameRate(SDL_Rect* bounds)
{
	_drawBounds = bounds;
}

void DrawableFrameRate::Update(float deltaMs) 
{
	if (_accumultedUpdateTime <= 1000)
	{
		_accumultedUpdateTime += deltaMs;
		_countUpdates++;
	}
	else
	{
		_framesPerSecond = _countUpdates;
		_accumultedUpdateTime = _countUpdates = 0;
	}
};

/// <summary>
/// All game objects can be drawn uniformly
/// </summary>
/// <param name="renderer"></param>
void DrawableFrameRate::Draw(SDL_Renderer* renderer) 
{
	RectDebugging::printInRect(renderer, std::to_string(_framesPerSecond) + " fps", _drawBounds, {0,0,0});	
}

