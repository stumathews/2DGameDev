#pragma once
#include "Actor.h"
#include <SDL.h>
#include "Drawing.h"
#include <iostream>
	
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

class Ball : public Actor
{
public:
	Ball(int x, int y, int w, int h, int posX, int posY)
		: Actor(x, y, w, h, posX, posY)
	{
		isTravelingLeft = false; // its traveling right
	}
	
	void Draw(SDL_Renderer* renderer)
	{
		Drawing::DrawRectangle(posX, posY,mBounds.w,mBounds.h, renderer);
	}
		

	void DoLogic()
	{
		
		if(isTravelingLeft)
		{
			if(posX == 0)
			{		
				isTravelingLeft = false;
			}
			MoveLeft();
		}
		else
		{
			if(posX == SCREEN_WIDTH)
			{		
				isTravelingLeft = true;
			}
			MoveRight();
		}	
	}
	Ball(){}
	virtual ~Ball();
private:
	bool isTravelingLeft;
};

