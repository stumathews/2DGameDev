#pragma once
#include "MoveStrategy.h"
#include <util/Tuple.h>
#include <Movement/Movement.h>
#include <movement/IMovement.h>

class Player;
class Room;

class PlayerMoveStrategy : public IPlayerMoveStrategy
{
public:
	PlayerMoveStrategy(std::shared_ptr<Player> player, int edgeBy);
	virtual bool MovePlayer(std::shared_ptr<gamelib::IMovement> movement) override;
	
private:
	std::shared_ptr<Player> player;
	bool debug;	
	void SetPlayerPosition(gamelib::coordinate<int> resultingMove);
	bool CanPlayerMove(gamelib::Direction direction, std::shared_ptr<gamelib::IMovement> movement);
	gamelib::coordinate<int> CalculatePlayerMove(std::shared_ptr<gamelib::IMovement> movement, int pixelsToMove);
	bool IsValidMove(std::shared_ptr<gamelib::IMovement> movement);
	bool ignoreRestrictions;
};

