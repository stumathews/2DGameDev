#pragma once
#include <util/Tuple.h>
#include <movement/IMovement.h>
#include "RoomInfo.h"
#include "movement/IGameMoveStrategy.h"

class Player;
class Room;

class GameObjectMoveStrategy final : public gamelib::IGameObjectMoveStrategy
{
public:
	GameObjectMoveStrategy(const std::shared_ptr<gamelib::GameObject>& gameObject, const std::shared_ptr<RoomInfo>& roomInfo);
	bool MoveGameObject(std::shared_ptr<gamelib::IMovement> movement) override;	
private:
	std::shared_ptr<gamelib::GameObject> gameObject;
	bool debug;
	std::shared_ptr<RoomInfo> roomInfo;
	void SetGameObjectPosition(gamelib::Coordinate<int> resultingMove) const;
	[[nodiscard]] bool CanGameObjectMove(gamelib::Direction direction) const;
	[[nodiscard]] gamelib::Coordinate<int> CalculateGameObjectMove(const std::shared_ptr<gamelib::IMovement>& movement, int pixelsToMove) const;
	[[nodiscard]] bool IsValidMove(const std::shared_ptr<gamelib::IMovement>& movement) const;
	bool ignoreRestrictions;
};
