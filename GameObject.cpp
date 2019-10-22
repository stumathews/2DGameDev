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
	
	GameObjectBase::DrawResource(renderer);
	// Custom drawing afterwards occus here
}
