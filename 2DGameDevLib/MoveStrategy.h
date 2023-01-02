#pragma once
#include "pch.h"
#include <memory>
#include <Direction.h>
#include <movement/IMovement.h>

class Room;
class Movement;

class IPlayerMoveStrategy
{
public:
	virtual ~IPlayerMoveStrategy() = default;
	/// <summary>
	/// Move the player according to the calculated movement
	/// </summary>
	/// <returns>true if movement could be performed, false otherwise</returns>
	virtual bool MovePlayer(std::shared_ptr<gamelib::IMovement> movement) = 0;
};
