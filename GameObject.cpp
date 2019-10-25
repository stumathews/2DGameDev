#include "GameObject.h"
#include "Events.h"
#include <memory>
#include "Event.h"
#include <iostream>

void GameObject::ProcessEvent(std::shared_ptr<Event> evt)
{
	if(evt->m_eventType == PositionChangeEventType)
	{
		auto event = std::dynamic_pointer_cast<PositionChangeEvent>(evt);
		if(event->m_direction == Up)					
			MoveUp();			
		
		if(event->m_direction == Down)
			MoveDown();			
		
		if(event->m_direction == Left)
			MoveLeft();			
		
		if(event->m_direction == Right)
			MoveRight();		
	}
	if(evt->m_eventType == DoLogicUpdateEventType)
		VDoLogic();

}

void GameObject::VDraw(SDL_Renderer * renderer)
{
	// Include base drawing functionality
	
	DrawResource(renderer);
	// Custom drawing afterwards occus here
}

void GameObject::DetectSideColission()
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
	


void GameObject::DrawResource(SDL_Renderer* renderer)
{
	auto resource = GetResource();
	if(resource != NULL && resource->m_type._Equal("graphic") /*Blit only resources to the screen*/)
	{
		SDL_Rect drawLocation = { m_xPos, m_yPos, 100,100 };	
		auto rect = GetResource()->m_bIsAnimated 
					?  &m_GraphicsResource->m_viewPort 
					: NULL;
		SDL_BlitSurface(m_GraphicsResource->m_Surface, rect, SDLGraphicsManager::GetInstance().m_WindowSurface, &drawLocation);	
	}
}