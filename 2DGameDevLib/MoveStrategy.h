#pragma once
#include "pch.h"
#include <memory>

class Room;
class Movement;

class IPlayerMoveStrategy
{
public:
	// Move the player according to the calculated movement
	virtual void MovePlayer(std::shared_ptr<Movement> movement) = 0;
	virtual bool CanPlayerMoveUp(std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& aboveRoom, std::shared_ptr<Movement> movement) = 0;
	virtual bool CanPlayerMoveDown(std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& bottomRoom,std::shared_ptr<Movement> movement) = 0;
	virtual bool CanPlayerMoveLeft(std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& leftRoom, std::shared_ptr<Movement> movement) = 0;
	virtual bool CanPlayerMoveRight(std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& rightRoom, std::shared_ptr<Movement> movement) = 0;
};
