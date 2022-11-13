#pragma once
#include "pch.h"
#include <memory>
#include <Direction.h>

class Room;
class Movement;

class IPlayerMoveStrategy
{
public:
	/// <summary>
	/// Move the player according to the calculated movement
	/// </summary>
	/// <returns>true if movement could be performed, false otherwise</returns>
	virtual bool MovePlayer(std::shared_ptr<Movement> movement) = 0;
};
