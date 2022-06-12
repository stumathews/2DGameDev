#pragma once
#include "MoveStrategy.h"
#include "Player.h"
#include "Room.h"
#include <util/Tuple.h>

class SnapToRoomStrategy : public IMoveStrategy
{
public:
	SnapToRoomStrategy(std::shared_ptr<Player> player);
	virtual void MoveTo(std::shared_ptr<Room> room) override;
	virtual void MoveTo(std::shared_ptr<Room> room, std::shared_ptr<Movement> movement) override;

private:
	void CenterPlayerInRoom(std::shared_ptr<Room> room);
	std::shared_ptr<Player> player;

	// Inherited via IMoveStrategy
	virtual bool CanMoveUp(const bool& isMovingUp, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& aboveRoom) override;
	virtual bool CanMoveDown(const bool& isMovingDown, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& bottomRoom) override;
	virtual bool CanMoveLeft(const bool& isMovingLeft, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& leftRoom) override;
	virtual bool CanMoveRight(const bool& isMovingRight, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& rightRoom) override;
};

