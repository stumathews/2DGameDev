#include "Ball.h"
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
	
	// Surely this should be a more common routine, that all Actors should be able to do by default (draw themselves)

	// Switch in diffirence keyframes here

	SDL_Rect drawLocation = { m_xPos, m_yPos };	
	
	SDL_BlitSurface(m_GraphicsResource->m_Surface, &m_GraphicsResource->m_Surface->clip_rect, GraphicsManager::GetInstance().m_WindowSurface, &drawLocation);
	
}
