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

private:
	void CenterPlayerInRoom(std::shared_ptr<Room> room);
	std::shared_ptr<Player> player;
};

