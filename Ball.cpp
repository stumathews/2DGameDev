#include "Ball.h"
#include "Events.h"
#include <memory>
#include "Event.h"
#include <iostream>

void Ball::ProcessEvent(std::shared_ptr<Event> evt)
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

void Ball::VDraw(SDL_Renderer * renderer)
{
	//Drawing::DrawRectangle(posX, posY,mBounds.w,mBounds.h, renderer, red, blue, green);
	SDL_Rect drect;
	drect.x = posX;
	drect.y = posY;
	SDL_BlitSurface(m_GraphicsResource->m_Surface, NULL, GraphicsManager::GetInstance().m_WindowSurface, &drect);
	
}
