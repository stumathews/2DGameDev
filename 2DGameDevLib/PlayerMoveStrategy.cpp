#include "pch.h"
#include "Player.h"
#include "Room.h"
#include "scene/SceneManager.h"
#include "PlayerMoveStrategy.h"
#include <Direction.h>
#include <common/Logger.h>
#include <sstream>
#include <exceptions/EngineException.h>
#include <util/SettingsManager.h>
#include "../game/LevelManager.h"
#include "Level.h"
#include "GameData.h"

PlayerMoveStrategy::PlayerMoveStrategy(std::shared_ptr<Player> inPlayer, int edgeIncrement)
{
	player = inPlayer;
	debug = gamelib::SettingsManager::Get()->GetBool("player", "debugMovement");
	ignoreRestrictions = gamelib::SettingsManager::Get()->GetBool("player", "ignoreRestrictions");
}

bool PlayerMoveStrategy::MovePlayer(std::shared_ptr<gamelib::IMovement> movement)
{
	auto isMoveValid = false;
	if (IsValidMove(movement) || ignoreRestrictions) 
	{
		SetPlayerPosition(CalculatePlayerMove(movement, movement->GetPixelsToMove()));
		isMoveValid = true;
	}
	return isMoveValid;
}

gamelib::coordinate<int> PlayerMoveStrategy::CalculatePlayerMove(std::shared_ptr<gamelib::IMovement> movement, int pixelsToMove)
{		
	int resulting_x;	
	int resulting_y; 
	
	resulting_y = player->Position.GetY();
	resulting_x = player->Position.GetX();

	switch(movement->GetDirection())
	{
		case gamelib::Direction::Down:  resulting_y += pixelsToMove; break;	
		case gamelib::Direction::Up:    resulting_y -= pixelsToMove; break;
		case gamelib::Direction::Left:  resulting_x -= pixelsToMove; break;
		case gamelib::Direction::Right: resulting_x += pixelsToMove; break;
	}
	
	return gamelib::coordinate<int>(resulting_x, resulting_y);
}

void PlayerMoveStrategy::SetPlayerPosition(gamelib::coordinate<int> resultingMove)
{
	player->Position.SetX(resultingMove.GetX());
	player->Position.SetY(resultingMove.GetY());
}

bool PlayerMoveStrategy::IsValidMove(std::shared_ptr<gamelib::IMovement> movement)
{
	if (ignoreRestrictions)
	{
		return true;
	}
	
	auto currentRoom = player->GetCurrentRoom();
	auto topRoom = player->GetTopRoom();
	auto rightRoom = player->GetRightRoom();
	auto bottomRoom = player->GetBottomRoom();
	auto leftRoom = player->GetLeftRoom();

	switch (movement->GetDirection())
	{
	case gamelib::Direction::Down:
		return CanPlayerMove(gamelib::Direction::Down, movement);
		break;
	case gamelib::Direction::Left:
		return CanPlayerMove(gamelib::Direction::Left, movement);
		break;
	case gamelib::Direction::Right:
		return CanPlayerMove(gamelib::Direction::Right, movement);
		break;
	case gamelib::Direction::Up:
		return CanPlayerMove(gamelib::Direction::Up, movement);
		break;
	default:
		return false;
		break;
	}
	return false;
}


bool PlayerMoveStrategy::CanPlayerMove(gamelib::Direction direction, std::shared_ptr<gamelib::IMovement> movement)
{
	std::shared_ptr<Room> targetRoom = nullptr;
	bool touchingBlockingWalls = false, hasValidTargetRoom = false;
	auto currentRoom = player->GetCurrentRoom();
	
	auto IntersectsRectAndLine = [=](SDL_Rect bounds, gamelib::Line line) -> bool {
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

	return !touchingBlockingWalls;}

