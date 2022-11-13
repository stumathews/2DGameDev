#pragma once
#include "MoveStrategy.h"
#include <util/Tuple.h>
#include <Movement/Movement.h>

class Player;
class Room;

class PlayerMoveStrategy : public IPlayerMoveStrategy
{
public:
	PlayerMoveStrategy(std::shared_ptr<Player> player, int edgeBy);

	virtual bool MovePlayer(std::shared_ptr<Movement> movement) override;
	
private:
	std::shared_ptr<Player> player;
	bool debug;	
	bool WouldPlayerHotspotHitRoomInnerBounds(std::shared_ptr<Room>& room, std::shared_ptr<Movement> movement);
	void SetPlayerPosition(gamelib::coordinate<int> resultingMove);
	bool CanPlayerMove(gamelib::Direction direction, std::shared_ptr<Movement> movement);
	gamelib::coordinate<int> CalculatePlayerMove(std::shared_ptr<Movement> movement, int pixelsToMove);
	bool IsValidMove(std::shared_ptr<Movement> movement);
	bool ignoreRestrictions;
};

