#pragma once
#include <SDL.h>
#include "EventSubscriber.h"
#include "GraphicsResource.h"

class Actor : public EventSubscriber
{
public:
	Actor(){}
	Actor(int x, int y, int w, int h, int posX, int posY) 
		: posX(posX), posY(posY)
	{
		mBounds = { x = x, y = y, w = w, h = h};	
	}
	virtual void MoveUp()
	{
		posY -= moveInterval;
	}
	virtual void MoveDown()
	{
		posY += moveInterval;
	}
	virtual void MoveLeft()
	{
		posX -= moveInterval;
	}
	virtual void MoveRight()
	{
		posX += moveInterval;
	}

	virtual void DoLogic() = 0;

	virtual void Draw(SDL_Renderer* renderer) = 0;
	virtual ~Actor();

	// Set the graphics resource that underlies this Actor
	// There could be many Actors refering to this graphics resource in the game
	void SetGraphicsResource(GraphicsResource *graphicsResource);

protected:
	GraphicsResource* m_GraphicsResource;
	SDL_Rect mBounds;
	int posX;
	int posY;
	bool m_Visible;
	SDL_Color m_ColorKey;
	int moveInterval = 10; // move by intervals of 10 pixels

};

