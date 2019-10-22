#pragma once
#include "GameObjectBase.h"
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

/*
	A game Object 
*/
class GameObject : public GameObjectBase
{
public:
	GameObject() : red(0x00), blue(0xFF), green(0x00), isTravelingLeft(false) {}
	GameObject(int m_xPos, int m_yPos) 	: GameObjectBase(m_xPos, m_yPos)
	{
		red = 0x00;
		blue = 0xFF;
		green = 0x00;
		isTravelingLeft = false; // its traveling right		
	}

	/*
	* Deals with events that a ball knows how to work with
	*/
	void ProcessEvent(std::shared_ptr<Event> event);	

	// A game object should know how to draw itself
	void virtual VDraw(SDL_Renderer* renderer) = 0;	

	void virtual VDoLogic() = 0;

	void DetectSideColission()
	{
		if (isTravelingLeft)
		{
			if (m_xPos == 0) {
				isTravelingLeft = false;
				Mix_PlayChannel(-1, gLow, 0);
			}
		}
		else
		{
			if (m_xPos == SDLGraphicsManager::GetInstance().GetScreenWidth())
			{
				Mix_PlayChannel(-1, gLow, 0);
				isTravelingLeft = true;
			}
		}
	}
	
	virtual ~GameObject(){}
private:
	bool isTravelingLeft;
	int red, blue, green;
};

