#pragma once
#include "MoveStrategy.h"
#include <util/Tuple.h>
#include <Movement/Movement.h>

class Player;
class Room;

class EdgeTowardsRoomStrategy : public IMoveStrategy
{
public:
	EdgeTowardsRoomStrategy(std::shared_ptr<Player> player, int edgeBy);

    // Inherited via IMoveStrategy
    virtual void MoveTo(std::shared_ptr<Room> room) override;
	virtual void MoveTo(std::shared_ptr<Room> room, std::shared_ptr<Movement> movement) override;
private:
	std::shared_ptr<Player> player;
	int edgeIncrement;

	// Inherited via IMoveStrategy
	virtual bool CanMoveUp(const bool& isMovingUp, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& aboveRoom) override;
	bool WouldHitInnerBounds(std::shared_ptr<Room>& bottomRoom);
	virtual bool CanMoveDown(const bool& isMovingDown, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& bottomRoom) override;
	virtual bool CanMoveLeft(const bool& isMovingLeft, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& leftRoom) override;
	virtual bool CanMoveRight(const bool& isMovingRight, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& rightRoom) override;
	bool IsTerminalRooms(std::shared_ptr<Room> room1, std::shared_ptr<Room> room2);
	gamelib::coordinate<int> CalculatePlayerMoveTo(std::shared_ptr<Room> room, int moveInterval = 3);
	gamelib::Direction GetDirectionTowardsRoom(std::shared_ptr<Room> room);
};

