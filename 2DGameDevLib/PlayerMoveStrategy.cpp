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

PlayerMoveStrategy::PlayerMoveStrategy(std::shared_ptr<Player> player, int edgeIncrement)
{
	this->player = player;
	this->debug = gamelib::SettingsManager::Get()->GetBool("player", "debugMovement");
	this->ignoreRestrictions = gamelib::SettingsManager::Get()->GetBool("player", "ignoreRestrictions");
}

bool PlayerMoveStrategy::MovePlayer(std::shared_ptr<Movement> movement)
{
	if (!IsValidMove(movement))
		return false;

	SetPlayerPosition(CalculatePlayerMove(movement, movement->TakePixelsToMove()));
	return true;
}

gamelib::coordinate<int> PlayerMoveStrategy::CalculatePlayerMove(std::shared_ptr<Movement> movement, int pixelsToMove)
{		
	int resulting_x;	
	int resulting_y; 
	
	resulting_y = player->Position.GetY();
	resulting_x = player->Position.GetX();

	switch(movement->direction)
	{
	case gamelib::Direction::Down:
		resulting_y += pixelsToMove;
		break;	
	case gamelib::Direction::Up:
		resulting_y -= pixelsToMove;
		break;
	case gamelib::Direction::Left:
		resulting_x -= pixelsToMove;
		break;
	case gamelib::Direction::Right:
		resulting_x += pixelsToMove;
		break;
	}
	
	return gamelib::coordinate<int>(resulting_x, resulting_y);
}

void PlayerMoveStrategy::SetPlayerPosition(gamelib::coordinate<int> resultingMove)
{
	player->Position.SetX(resultingMove.GetX());
	player->Position.SetY(resultingMove.GetY());
}




bool PlayerMoveStrategy::WouldPlayerHotspotHitRoomInnerBounds(std::shared_ptr<Room>& room, std::shared_ptr<Movement> movement)
{
	// Calculate where the player would be if the move is done
	auto mockPlayerPosition = CalculatePlayerMove(movement, movement->PreviewPixelsToMove());

	// Calculate the location of the hotspot at that location
	auto mockPlayerHotSpot = player->Hotspot->CalculateHotspotPosition(mockPlayerPosition.GetX(), mockPlayerPosition.GetY());

	// Setup the bounds for the simulated hotspot
	auto mockPlayerHotSpotBounds = SDL_Rect { mockPlayerHotSpot.GetX(), mockPlayerHotSpot.GetY(), player->GetHotSpotLength() , player->GetHotSpotLength() };
	
	// Check if the player would intersect with the inner bounds of the room
	SDL_Rect result;
	auto isWithinRoom = SDL_IntersectRect(&room->InnerBounds, &mockPlayerHotSpotBounds, &result) == SDL_TRUE ? true : false;

	return isWithinRoom;
}


bool PlayerMoveStrategy::IsValidMove(std::shared_ptr<Movement> movement)
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

	switch (movement->direction)
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


bool PlayerMoveStrategy::CanPlayerMove(gamelib::Direction direction, std::shared_ptr<Movement> movement)
{
	std::shared_ptr<Room> targetRoom = nullptr;
	bool hasBlockingWalls = false, hasValidTargetRoom = false;
	auto currentRoom = player->GetCurrentRoom();

	if (direction == gamelib::Direction::Right)
	{
		targetRoom = player->GetRightRoom();
		hasValidTargetRoom = targetRoom != nullptr;
		hasBlockingWalls = (!hasValidTargetRoom || targetRoom->HasLeftWall()) && currentRoom->HasRightWall();
	}
	else if (direction == gamelib::Direction::Left)
	{
		targetRoom = player->GetLeftRoom();
		hasValidTargetRoom = targetRoom != nullptr;
		hasBlockingWalls = (!hasValidTargetRoom || targetRoom->HasRightWall()) && currentRoom->HasLeftWall();
	}
	else if (direction == gamelib::Direction::Up)
	{
		targetRoom = player->GetTopRoom();
		hasValidTargetRoom = targetRoom != nullptr;
		hasBlockingWalls = (!hasValidTargetRoom || targetRoom->HasBottomWall() && currentRoom->HasTopWall());
	}
	else if (direction == gamelib::Direction::Down)
	{
		targetRoom = player->GetBottomRoom();
		hasValidTargetRoom = targetRoom != nullptr;
		hasBlockingWalls = (!hasValidTargetRoom || targetRoom->HasTopWall()) && currentRoom->HasBottomWall();
	}

	auto isWithinCurrentRoom = WouldPlayerHotspotHitRoomInnerBounds(currentRoom, movement);
	auto isInNoMansLand = !isWithinCurrentRoom && (hasValidTargetRoom && !WouldPlayerHotspotHitRoomInnerBounds(targetRoom, movement));

	return isWithinCurrentRoom || isInNoMansLand || (!hasBlockingWalls);
}