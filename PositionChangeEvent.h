#pragma once
#include "Event.h"
enum Direction {Up, Down, Left, Right};
class PositionChangeEvent : public Event
{
public:
	PositionChangeEvent(Direction dir) : Event(PositionChangeEventType){
		SetData((void*) dir);
		m_direction = dir;
	}
	Direction m_direction;
	~PositionChangeEvent();
};

