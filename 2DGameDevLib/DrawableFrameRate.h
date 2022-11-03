#pragma once
#include <objects/GameObject.h>
#include "objects/DrawableGameObject.h"

class DrawableFrameRate : public gamelib::DrawableGameObject
{
public:
	DrawableFrameRate(SDL_Rect* bounds);
	void virtual Update(float deltaMs) override;

	/// <summary>
	/// All game objects can be drawn uniformly
	/// </summary>
	/// <param name="renderer"></param>
	void virtual Draw(SDL_Renderer* renderer) override;

	/// <summary>
	/// Every game Object needs to identify what type of game object it is
	/// </summary>
	/// <returns></returns>
	virtual gamelib::GameObjectType GetGameObjectType() override;
private:
	float accumultedUpdateTime = 0;
	unsigned int countUpdates = 0;
	unsigned int framesPerSecond = 0;
	SDL_Rect* drawBounds = {};
};