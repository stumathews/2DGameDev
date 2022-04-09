#pragma once
#include <memory>

class Room;

class IMoveStrategy
{
public:
	virtual void MoveTo(std::shared_ptr<Room> room) = 0;
};
