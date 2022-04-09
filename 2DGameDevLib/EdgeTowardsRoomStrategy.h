#pragma once
#include "MoveStrategy.h"

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
};

