#pragma once
#include "MoveStrategy.h"
#include <util/Tuple.h>
#include <Movement/Movement.h>

class Player;
class Room;

class EdgeTowardsRoomStrategy : public IPlayerMoveStrategy
{
public:
	EdgeTowardsRoomStrategy(std::shared_ptr<Player> player, int edgeBy);

    // Inherited via IMoveStrategy
	void SetPlayerPosition(gamelib::coordinate<int> resultingMove);
	virtual void MovePlayer(std::shared_ptr<Movement> movement) override;
private:
	std::shared_ptr<Player> player;
	int edgeIncrement;
	bool debug;

	// Inherited via IMoveStrategy
	virtual bool CanPlayerMoveUp(std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& aboveRoom, std::shared_ptr<Movement> movement) override;	
	virtual bool CanPlayerMoveDown( std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& bottomRoom, std::shared_ptr<Movement> movement) override;
	virtual bool CanPlayerMoveLeft(std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& leftRoom, std::shared_ptr<Movement> movement) override;
	virtual bool CanPlayerMoveRight( std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& rightRoom, std::shared_ptr<Movement> movement) override;
	bool WouldPlayerHotspotHitRoomInnerBounds(std::shared_ptr<Room>& bottomRoom, std::shared_ptr<Movement> movement);
	gamelib::coordinate<int> CalculatePlayerMove(std::shared_ptr<Movement> movement, int pixelsToMove);
	gamelib::Direction GetDirectionTowardsRoom(std::shared_ptr<Room> room);
};

