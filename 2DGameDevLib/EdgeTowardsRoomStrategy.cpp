#include "pch.h"
#include "Player.h"
#include "Room.h"
#include "scene/SceneManager.h"
#include "EdgeTowardsRoomStrategy.h"
#include <Direction.h>
#include <common/Logger.h>
#include <sstream>
#include <exceptions/EngineException.h>
#include <util/SettingsManager.h>

EdgeTowardsRoomStrategy::EdgeTowardsRoomStrategy(std::shared_ptr<Player> player, int edgeIncrement)
{
	this->player = player;
	this->edgeIncrement = edgeIncrement;
	this->debug = gamelib::SettingsManager::Get()->GetBool("player", "debugMovement");
}

gamelib::Direction EdgeTowardsRoomStrategy::GetDirectionTowardsRoom(std::shared_ptr<Room> targetRoom)
{
	auto moveDirection = gamelib::Direction::None;
	auto roomNumber = targetRoom->GetRoomNumber();
	auto playersRoom = player->GetCurrentRoom();

	if(playersRoom->GetNeighbourIndex(Side::Bottom) == roomNumber)
	{
		moveDirection = gamelib::Direction::Down;
	}
	else if(playersRoom->GetNeighbourIndex(Side::Top) == roomNumber)
	{
		moveDirection = gamelib::Direction::Up;
	}

	else if(playersRoom->GetNeighbourIndex(Side::Left) == roomNumber)
	{
		moveDirection = gamelib::Direction::Left;
	}
	else if(playersRoom->GetNeighbourIndex(Side::Right) == roomNumber)
	{
		moveDirection = gamelib::Direction::Right;
	}
	else
	{
		std::stringstream message;
		if (debug)
		{
			message << "The room number :" 
				    << roomNumber 
				    << " not in the neighbourhood of the player. Cannot determine direction towards targetted room.";

			gamelib::Logger::Get()->LogThis(message.str());
		}

		moveDirection = gamelib::Direction::None;
	}

	return moveDirection;
}

gamelib::coordinate<int> EdgeTowardsRoomStrategy::CalculatePlayerMoveTo(std::shared_ptr<Room> room, int move_interval)
{		
	auto directionTowardsRoom = GetDirectionTowardsRoom(room);
	int resulting_x;	
	int resulting_y; 
	
	resulting_y = player->Position.GetY();
	resulting_x = player->Position.GetX();

	switch(directionTowardsRoom)
	{
	case gamelib::Direction::Down:
		resulting_y += move_interval;
		break;	
	case gamelib::Direction::Up:
		resulting_y -= move_interval;
		break;
	case gamelib::Direction::Left:
		resulting_x -= move_interval;
		break;
	case gamelib::Direction::Right:
		resulting_x += move_interval;
		break;
	}
	
	return gamelib::coordinate<int>(resulting_x, resulting_y);
}

void EdgeTowardsRoomStrategy::MoveTo(std::shared_ptr<Room> room)
{
	// Edge player towards the room
	auto resultingMove = CalculatePlayerMoveTo(room);

	// Set the player's new position
	SetPlayerPosition(resultingMove);
}

void EdgeTowardsRoomStrategy::SetPlayerPosition(gamelib::coordinate<int> resultingMove)
{
	player->Position.SetX(resultingMove.GetX());
	player->Position.SetY(resultingMove.GetY());
}

void EdgeTowardsRoomStrategy::MoveTo(std::shared_ptr<Room> targetRoom, std::shared_ptr<Movement> movement)
{
	if (!player->IsValidMove(GetDirectionTowardsRoom(targetRoom)))
		return;

	SetPlayerPosition(CalculatePlayerMoveTo(targetRoom, movement->TakePixelsToMove()));	
}


bool EdgeTowardsRoomStrategy::WouldHitInnerBounds(std::shared_ptr<Room>& room)
{
	// Calculate where the player would be if the move is done
	auto mockPlayerPosition = CalculatePlayerMoveTo(room);

	// Calculate the location of the hotspot at that location
	auto mockPlayerHotSpot = player->CalculateHotspotPosition(mockPlayerPosition.GetX(), mockPlayerPosition.GetY());

	// Setup the bounds for the simulated hotspot
	auto mockPlayerHotSpotBounds = SDL_Rect { mockPlayerHotSpot.GetX(), mockPlayerHotSpot.GetY(), 1, 1 };
	
	// Check if the player would intersect with the inner bounds of the room
	SDL_Rect result;
	return SDL_IntersectRect(&room->InnerBounds, &mockPlayerHotSpotBounds, &result);
}

bool EdgeTowardsRoomStrategy::IsTerminalRooms(std::shared_ptr<Room> room1, std::shared_ptr<Room> room2)
{
	return room1->GetRoomNumber() == room2->GetRoomNumber();
}

bool EdgeTowardsRoomStrategy::CanMoveUp(const bool& isMovingUp, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& aboveRoom)
{
	if(!isMovingUp)
		return false;

	auto hasBlockingWalls = aboveRoom->HasBottomWall() && currentRoom->HasTopWall();
	return IsTerminalRooms(aboveRoom, currentRoom) ?  WouldHitInnerBounds(currentRoom) : !hasBlockingWalls || !WouldHitInnerBounds(aboveRoom);
}


bool EdgeTowardsRoomStrategy::CanMoveDown(const bool& isMovingDown, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& bottomRoom)
{
	if(!isMovingDown)
		return false;

	auto hasNoBlockingWalls = !bottomRoom->HasTopWall() && !currentRoom->HasBottomWall();
	return IsTerminalRooms(bottomRoom, currentRoom) 
		? WouldHitInnerBounds(currentRoom) 
		: hasNoBlockingWalls || !WouldHitInnerBounds(bottomRoom);
}

bool EdgeTowardsRoomStrategy::CanMoveLeft(const bool& isMovingLeft, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& leftRoom)
{
	if(!isMovingLeft)
		return false;

	auto hasNoBlockingWalls = !leftRoom->HasRightWall() && !currentRoom->HasLeftWall();
	return IsTerminalRooms(leftRoom, currentRoom) 
		? WouldHitInnerBounds(currentRoom) 
		: hasNoBlockingWalls || !WouldHitInnerBounds(leftRoom);
}

bool EdgeTowardsRoomStrategy::CanMoveRight(const bool& isMovingRight, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& rightRoom)
{
	if(!isMovingRight)
		return false;

	auto hasNoBlockingWalls = !rightRoom->HasLeftWall() && !currentRoom->HasRightWall();
	return IsTerminalRooms(rightRoom, currentRoom) 
		? WouldHitInnerBounds(currentRoom) 
		: hasNoBlockingWalls || !WouldHitInnerBounds(rightRoom);
}
