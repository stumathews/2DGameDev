#pragma once
#include "MoveStrategy.h"
#include <util/Tuple.h>
#include <movement/IMovement.h>

class Player;
class Room;

class PlayerMoveStrategy final : public IPlayerMoveStrategy
{
public:
	PlayerMoveStrategy(const std::shared_ptr<Player>& player, int edgeBy);
	bool MovePlayer(std::shared_ptr<gamelib::IMovement> movement) override;
	
private:
	std::shared_ptr<Player> player;
	bool debug;	
	void SetPlayerPosition(gamelib::Coordinate<int> resultingMove) const;
	[[nodiscard]] bool CanPlayerMove(gamelib::Direction direction, const std::shared_ptr<gamelib::IMovement>& movement) const;
	[[nodiscard]] gamelib::Coordinate<int> CalculatePlayerMove(const std::shared_ptr<gamelib::IMovement>& movement, int pixelsToMove) const;
	bool IsValidMove(const std::shared_ptr<gamelib::IMovement>& movement) const;
	bool ignoreRestrictions;
};

