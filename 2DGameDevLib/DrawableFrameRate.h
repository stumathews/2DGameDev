#pragma once
#include <objects/GameObject.h>
#include "objects/DrawableGameObject.h"

/// <summary>
/// Drawable Frame Rate
/// </summary>
class DrawableFrameRate : public gamelib::DrawableGameObject
{
public:

	DrawableFrameRate(SDL_Rect* bounds);

	/// <summary>
	/// Update the framerate
	/// </summary>
	/// <param name="deltaMs"></param>
	void virtual Update(float deltaMs) override;

	/// <summary>
	/// All game objects can be drawn uniformly
	/// </summary>
	/// <param name="renderer"></param>
	void virtual Draw(SDL_Renderer* renderer) override;


	/// Every game Object needs to identify what type of game object it is
	/// </summary>
	/// <returns></returns>
	virtual gamelib::GameObjectType GetGameObjectType() override { return gamelib::GameObjectType::GameDefined; }

private:
	float _accumultedUpdateTime = 0;
	unsigned int _countUpdates = 0;
	unsigned int _framesPerSecond = 0;
	SDL_Rect* _drawBounds = {};
};