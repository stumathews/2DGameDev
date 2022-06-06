#pragma once
#include "MoveStrategy.h"
#include <util/Tuple.h>

class Player;
class Room;

class EdgeTowardsRoomStrategy : public IMoveStrategy
{
public:
	EdgeTowardsRoomStrategy(std::shared_ptr<Player> player, int edgeBy);

    // Inherited via IMoveStrategy
    virtual void MoveTo(std::shared_ptr<Room> room) override;
private:
	std::shared_ptr<Player> player;
	int edgeIncrement;

	// Inherited via IMoveStrategy
	virtual bool CanMoveUp(const bool& isMovingUp, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& aboveRoom) override;
	bool WouldHitInnerBounds(std::shared_ptr<Room>& bottomRoom);
	bool IsTerminalRooms(std::shared_ptr<Room> room1, std::shared_ptr<Room> room2);
	virtual bool CanMoveDown(const bool& isMovingDown, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& bottomRoom) override;
	virtual bool CanMoveLeft(const bool& isMovingLeft, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& leftRoom) override;
	virtual bool CanMoveRight(const bool& isMovingRight, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& rightRoom) override;

	gamelib::coordinate<int> CalculatePlayerMoveTo(std::shared_ptr<Room> room);
};

