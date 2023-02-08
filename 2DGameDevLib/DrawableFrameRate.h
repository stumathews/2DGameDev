#pragma once
#include <objects/GameObject.h>
#include "objects/DrawableGameObject.h"

class DrawableFrameRate final : public gamelib::DrawableGameObject
{
public:
	explicit DrawableFrameRate(SDL_Rect* bounds);

	/// <summary>
	/// Update the frame rate
	/// </summary>
	/// <param name="deltaMs"></param>
	void Update(unsigned long deltaMs) override;

	/// <summary>
	/// All game objects can be drawn uniformly
	/// </summary>
	/// <param name="renderer"></param>
	void Draw(SDL_Renderer* renderer) override;


	/// <summary>
	/// Every game Object needs to identify what type of game object it is
	/// </summary>
	/// <returns></returns>
	gamelib::GameObjectType GetGameObjectType() override { return gamelib::GameObjectType::GameDefined; }

private:
	unsigned long accumulatedUpdateTime = 0;
	unsigned int countUpdates = 0;
	unsigned int framesPerSecond = 0;
	SDL_Rect* drawBounds = {};
};