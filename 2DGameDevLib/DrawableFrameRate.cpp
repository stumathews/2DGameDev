#include "pch.h"
#include "DrawableFrameRate.h"
#include <util\RectDebugging.h>
#include <SDL.h>
#include <events\Event.h>

DrawableFrameRate::DrawableFrameRate(SDL_Rect* bounds)
{
	drawBounds = bounds;
}

void DrawableFrameRate::Update(float deltaMs) 
{
	if (accumultedUpdateTime <= 1000)
	{
		accumultedUpdateTime += deltaMs;
		countUpdates++;
	}
	else
	{
		framesPerSecond = countUpdates;
		accumultedUpdateTime = countUpdates = 0;
	}
};

/// <summary>
/// All game objects can be drawn uniformly
/// </summary>
/// <param name="renderer"></param>
void DrawableFrameRate::Draw(SDL_Renderer* renderer) 
{
	gamelib::RectDebugging::printInRect(renderer, std::to_string(framesPerSecond) + " fps", drawBounds, {0,0,0});
	
}

/// <summary>
/// Every game Object needs to identify what type of game object it is
/// </summary>
/// <returns></returns>
gamelib::GameObjectType DrawableFrameRate::GetGameObjectType() 
{
	return gamelib::GameObjectType::Undefined;
}