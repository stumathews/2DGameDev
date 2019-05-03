#pragma once
#include "Actor.h"
#include <SDL.h>
#include "Drawing.h"
#include <iostream>
#include <SDL_mixer.h>
#include "Event.h"
#include "EventSubscriber.h"
	
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern Mix_Chunk *gScratch;
extern Mix_Chunk *gHigh;
extern Mix_Chunk *gMedium;
extern Mix_Chunk *gLow;


class Ball : public Actor
{
public:
	Ball(int x, int y, int w, int h, int posX, int posY)
		: Actor(x, y, w, h, posX, posY)
	{
		red = 0xFF;
		blue = 0x00;
		green = 0x00;
		isTravelingLeft = false; // its traveling right
	}

	void ProcessEvent(std::shared_ptr<Event> event);
	
	void Draw(SDL_Renderer* renderer)
	{
		Drawing::DrawRectangle(posX, posY,mBounds.w,mBounds.h, renderer, red, blue, green);
	}
		

	void DoLogic()
	{		
		if(isTravelingLeft)
		{
			if(posX == 0)
			{		
				isTravelingLeft = false;
				Mix_PlayChannel( -1, gLow, 0 );
			}
			MoveLeft();
		}
		else
		{
			if(posX == SCREEN_WIDTH)
			{		
				Mix_PlayChannel( -1, gLow, 0 );
				isTravelingLeft = true;
			}
			MoveRight();
		}	
	}
	Ball(){}
	virtual ~Ball();
private:
	bool isTravelingLeft;
	int red, blue, green;
};

