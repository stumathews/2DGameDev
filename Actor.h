#pragma once
#include <SDL.h>

class Actor
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
protected:
	SDL_Rect mBounds;
	int posX;
	int posY;
	int moveInterval = 10; // move by intervals of 10 pixels

};

