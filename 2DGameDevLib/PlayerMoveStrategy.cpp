#include "pch.h"
#include "Player.h"
#include "Room.h"
#include "PlayerMoveStrategy.h"
#include <Direction.h>
#include <common/Logger.h>
#include <exceptions/EngineException.h>
#include <util/SettingsManager.h>
#include "../game/LevelManager.h"

PlayerMoveStrategy::PlayerMoveStrategy(const std::shared_ptr<Player>& inPlayer, int)
{
	player = inPlayer;
	debug = gamelib::SettingsManager::Get()->GetBool("player", "debugMovement");
	ignoreRestrictions = gamelib::SettingsManager::Get()->GetBool("player", "ignoreRestrictions");
}

bool PlayerMoveStrategy::MovePlayer(const std::shared_ptr<gamelib::IMovement> movement)
{
	auto isMoveValid = false;
	if (IsValidMove(movement)) 
	{
		SetPlayerPosition(CalculatePlayerMove(movement, movement->GetPixelsToMove()));
		isMoveValid = true;
	}
	return isMoveValid;
}

gamelib::coordinate<int> PlayerMoveStrategy::CalculatePlayerMove(const std::shared_ptr<gamelib::IMovement>& movement, const int pixelsToMove) const
{
	int resultingY = player->Position.GetY();
	int resultingX = player->Position.GetX();

	switch(movement->GetDirection())
	{
		case gamelib::Direction::Down:  resultingY += pixelsToMove; break;	
		case gamelib::Direction::Up:    resultingY -= pixelsToMove; break;
		case gamelib::Direction::Left:  resultingX -= pixelsToMove; break;
		case gamelib::Direction::Right: resultingX += pixelsToMove; break;
		case gamelib::Direction::None: THROW(0, "Direction is NOne", "PlayerMoveStrategy")
	}
	
	return {resultingX, resultingY};
}

void PlayerMoveStrategy::SetPlayerPosition(const gamelib::coordinate<int> resultingMove) const
{
	player->Position.SetX(resultingMove.GetX());
	player->Position.SetY(resultingMove.GetY());
}

bool PlayerMoveStrategy::IsValidMove(const std::shared_ptr<gamelib::IMovement>& movement) const
{
	if (ignoreRestrictions) { return true;}

	switch (movement->GetDirection())
	{
		case gamelib::Direction::Down: return CanPlayerMove(gamelib::Direction::Down, movement);
		case gamelib::Direction::Left: return CanPlayerMove(gamelib::Direction::Left, movement);
		case gamelib::Direction::Right: return CanPlayerMove(gamelib::Direction::Right, movement);
		case gamelib::Direction::Up: return CanPlayerMove(gamelib::Direction::Up, movement);
		case gamelib::Direction::None: THROW(0, "Direction is NOne", "PlayerMoveStrategy")
	}
	return false;
}

bool PlayerMoveStrategy::CanPlayerMove(const gamelib::Direction direction, const std::shared_ptr<gamelib::IMovement>&
                                       movement) const
{
	std::shared_ptr<Room> targetRoom;
	bool touchingBlockingWalls = false, hasValidTargetRoom;
	const auto currentRoom = player->GetCurrentRoom();
	
	auto IntersectsRectAndLine = [=](const SDL_Rect bounds, gamelib::Line line) -> bool 
	{
		return SDL_IntersectRectAndLine(&bounds, &line.x1, &line.y1, &line.x2, &line.y2);
	};
	
	if (direction == gamelib::Direction::Right)
	{
		targetRoom = player->GetRightRoom();
		hasValidTargetRoom = targetRoom != nullptr;
		touchingBlockingWalls = 
			(hasValidTargetRoom && targetRoom->HasLeftWall() && IntersectsRectAndLine(player->Bounds, targetRoom->LeftLine)) ||
			currentRoom->HasRightWall() && IntersectsRectAndLine(player->Bounds, currentRoom->RightLine);
	}
	else if (direction == gamelib::Direction::Left)
	{	
		targetRoom = player->GetLeftRoom();
		hasValidTargetRoom = targetRoom != nullptr;
		
		touchingBlockingWalls =
			(hasValidTargetRoom && targetRoom->HasRightWall() && IntersectsRectAndLine(player->Bounds, targetRoom->RightLine)) ||
			currentRoom->HasLeftWall() && IntersectsRectAndLine(player->Bounds, currentRoom->LeftLine);
	}
	else if (direction == gamelib::Direction::Up)
	{
		targetRoom = player->GetTopRoom();
		hasValidTargetRoom = targetRoom != nullptr;
		touchingBlockingWalls =
			(hasValidTargetRoom && targetRoom->HasBottomWall() && IntersectsRectAndLine(player->Bounds, targetRoom->BottomLine)) ||
			currentRoom->HasTopWall() && IntersectsRectAndLine(player->Bounds, currentRoom->TopLine);
	}
	else if (direction == gamelib::Direction::Down)
	{
		targetRoom = player->GetBottomRoom();
		hasValidTargetRoom = targetRoom != nullptr;
		touchingBlockingWalls =
			(hasValidTargetRoom && targetRoom->HasTopWall() && IntersectsRectAndLine(player->Bounds, targetRoom->TopLine)) ||
			currentRoom->HasBottomWall() && IntersectsRectAndLine(player->Bounds, currentRoom->BottomLine);
	}

	return !touchingBlockingWalls;
}

