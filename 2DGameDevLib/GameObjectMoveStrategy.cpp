#include "pch.h"
#include "GameObjectMoveStrategy.h"

#include "Player.h"
#include "Room.h"
#include <Direction.h>
#include <common/Logger.h>
#include <exceptions/EngineException.h>
#include <util/SettingsManager.h>
#include "../game/LevelManager.h"
#include "movement/IMovement.h"

GameObjectMoveStrategy::GameObjectMoveStrategy(const std::shared_ptr<gamelib::GameObject>& gameObject, const std::shared_ptr<RoomInfo>& roomInfo)
{
	this->gameObject = gameObject;
	this->roomInfo = roomInfo;
	debug = gamelib::SettingsManager::Get()->GetBool("player", "debugMovement");
	ignoreRestrictions = gamelib::SettingsManager::Get()->GetBool("player", "ignoreRestrictions");
}

bool GameObjectMoveStrategy::MoveGameObject(const std::shared_ptr<gamelib::IMovement> movement)
{
	auto isMoveValid = false;
	if (IsValidMove(movement)) 
	{
		SetGameObjectPosition(CalculateGameObjectMove(movement, movement->GetPixelsToMove()));
		isMoveValid = true;
	}
	return isMoveValid;
}

gamelib::Coordinate<int> GameObjectMoveStrategy::CalculateGameObjectMove(const std::shared_ptr<gamelib::IMovement>& movement, const int pixelsToMove) const
{
	int resultingY = gameObject->Position.GetY();
	int resultingX = gameObject->Position.GetX();

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

void GameObjectMoveStrategy::SetGameObjectPosition(const gamelib::Coordinate<int> resultingMove) const
{
	gameObject->Position.SetX(resultingMove.GetX());
	gameObject->Position.SetY(resultingMove.GetY());
}

bool GameObjectMoveStrategy::IsValidMove(const std::shared_ptr<gamelib::IMovement>& movement) const
{
	if (ignoreRestrictions) { return true;}

	switch (movement->GetDirection())
	{
		case gamelib::Direction::Down: return CanGameObjectMove(gamelib::Direction::Down);
		case gamelib::Direction::Left: return CanGameObjectMove(gamelib::Direction::Left);
		case gamelib::Direction::Right: return CanGameObjectMove(gamelib::Direction::Right);
		case gamelib::Direction::Up: return CanGameObjectMove(gamelib::Direction::Up);
		case gamelib::Direction::None: THROW(0, "Direction is NOne", "PlayerMoveStrategy")
	}
	return false;
}

bool GameObjectMoveStrategy::CanGameObjectMove(const gamelib::Direction direction) const
{
	std::shared_ptr<Room> targetRoom;
	bool touchingBlockingWalls = false, hasValidTargetRoom;
	const auto currentRoom = roomInfo->GetCurrentRoom();
	
	auto intersectsRectAndLine = [=](const SDL_Rect bounds, gamelib::Line line) -> bool 
	{
		return SDL_IntersectRectAndLine(&bounds, &line.X1, &line.Y1, &line.X2, &line.Y2);
	};
	
	if (direction == gamelib::Direction::Right)
	{
		targetRoom = roomInfo->GetRightRoom();
		hasValidTargetRoom = targetRoom != nullptr;
		touchingBlockingWalls = 
			(hasValidTargetRoom && targetRoom->HasLeftWall() && intersectsRectAndLine(gameObject->Bounds, targetRoom->LeftLine)) ||
			currentRoom->HasRightWall() && intersectsRectAndLine(gameObject->Bounds, currentRoom->RightLine);
	}
	else if (direction == gamelib::Direction::Left)
	{	
		targetRoom = roomInfo->GetLeftRoom();
		hasValidTargetRoom = targetRoom != nullptr;
		
		touchingBlockingWalls =
			(hasValidTargetRoom && targetRoom->HasRightWall() && intersectsRectAndLine(gameObject->Bounds, targetRoom->RightLine)) ||
			currentRoom->HasLeftWall() && intersectsRectAndLine(gameObject->Bounds, currentRoom->LeftLine);
	}
	else if (direction == gamelib::Direction::Up)
	{
		targetRoom = roomInfo->GetTopRoom();
		hasValidTargetRoom = targetRoom != nullptr;
		touchingBlockingWalls =
			(hasValidTargetRoom && targetRoom->HasBottomWall() && intersectsRectAndLine(gameObject->Bounds, targetRoom->BottomLine)) ||
			currentRoom->HasTopWall() && intersectsRectAndLine(gameObject->Bounds, currentRoom->TopLine);
	}
	else if (direction == gamelib::Direction::Down)
	{
		targetRoom = roomInfo->GetBottomRoom();
		hasValidTargetRoom = targetRoom != nullptr;
		touchingBlockingWalls =
			(hasValidTargetRoom && targetRoom->HasTopWall() && intersectsRectAndLine(gameObject->Bounds, targetRoom->TopLine)) ||
			currentRoom->HasBottomWall() && intersectsRectAndLine(gameObject->Bounds, currentRoom->BottomLine);
	}

	return !touchingBlockingWalls;
}

