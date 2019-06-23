#pragma once
#include "Actor.h"
#include <SDL.h>
#include "Drawing.h"
#include <iostream>
#include <SDL_mixer.h>
#include "Events.h"
#include "Event.h"
#include "GraphicsManager.h"

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
		red = 0x00;
		blue = 0xFF;
		green = 0x00;
		isTravelingLeft = false; // its traveling right		
	}

	void ProcessEvent(std::shared_ptr<Event> event);
	
	void VDraw(SDL_Renderer* renderer);
		

	void VDoLogic()
	{		
		if(isTravelingLeft)
		{
			if(posX == 0)
			{		
				isTravelingLeft = false;
				Mix_PlayChannel( -1, gLow, 0 );
			}
			//MoveLeft();
		}
		else
		{
			if(posX == GraphicsManager::GetInstance().GetScreenWidth())
			{		
				Mix_PlayChannel( -1, gLow, 0 );
				isTravelingLeft = true;
			}
			//MoveRight();
		}	
	}
	Ball(){}
	virtual ~Ball(){}
private:
	bool isTravelingLeft;
	int red, blue, green;
};

