#include "Ball.h"
#include "Events.h"
#include <memory>
#include "Event.h"
#include <iostream>

void Ball::ProcessEvent(std::shared_ptr<Event> evt)
{
	// Handle PositionChangeEvents
	if(evt->m_eventType == PositionChangeEventType)
	{
		std::shared_ptr<PositionChangeEvent> cpe = std::dynamic_pointer_cast<PositionChangeEvent>(evt);
		if(cpe->m_direction == Up){			
			MoveUp();
			std::cout << "Move up event processed" << std::endl;
		}
		if(cpe->m_direction == Down){
			MoveDown();
			std::cout << "Move down event processed" << std::endl;
		}
		if(cpe->m_direction == Left){
			MoveLeft();
			std::cout << "Move left event processed" << std::endl;
		}
		if(cpe->m_direction == Right){
			MoveRight();
			std::cout << "Move right event processed" << std::endl;
		}
	}
	if(evt->m_eventType == DoLogicUpdateEventType)
	{
		VDoLogic();
	}
}

Ball::~Ball()
{
	
}
