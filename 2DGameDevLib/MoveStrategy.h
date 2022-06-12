#pragma once
#include "pch.h"
#include <memory>

class Room;
class Movement;

class IMoveStrategy
{
public:
	virtual void MoveTo(std::shared_ptr<Room> room) = 0;
	virtual void MoveTo(std::shared_ptr<Room> room, std::shared_ptr<Movement> movement) = 0;
	virtual bool CanMoveUp(const bool& isMovingUp, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& aboveRoom) = 0;
	virtual bool CanMoveDown(const bool& isMovingDown, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& bottomRoom) = 0;
	virtual bool CanMoveLeft(const bool& isMovingLeft, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& leftRoom) = 0;
	virtual bool CanMoveRight(const bool& isMovingRight, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& rightRoom) = 0;
};
