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
#include "../game/LevelManager.h"
#include "Level.h"

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

gamelib::coordinate<int> EdgeTowardsRoomStrategy::CalculatePlayerMove(std::shared_ptr<Movement> movement, int pixelsToMove)
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

void EdgeTowardsRoomStrategy::SetPlayerPosition(gamelib::coordinate<int> resultingMove)
{
	player->Position.SetX(resultingMove.GetX());
	player->Position.SetY(resultingMove.GetY());
}

void EdgeTowardsRoomStrategy::MovePlayer( std::shared_ptr<Movement> movement)
{
	if (!player->IsValidMove(movement))
		return;

	SetPlayerPosition(CalculatePlayerMove(movement, movement->TakePixelsToMove()));
}


bool EdgeTowardsRoomStrategy::WouldPlayerHotspotHitRoomInnerBounds(std::shared_ptr<Room>& room, std::shared_ptr<Movement> movement)
{
	// Calculate where the player would be if the move is done
	auto mockPlayerPosition = CalculatePlayerMove(movement, movement->PreviewPixelsToMove());

	// Calculate the location of the hotspot at that location
	auto mockPlayerHotSpot = player->CalculateHotspotPosition(mockPlayerPosition.GetX(), mockPlayerPosition.GetY());

	// Setup the bounds for the simulated hotspot
	auto mockPlayerHotSpotBounds = SDL_Rect { mockPlayerHotSpot.GetX(), mockPlayerHotSpot.GetY(), player->GetHotSpotLength() , player->GetHotSpotLength() };
	
	// Check if the player would intersect with the inner bounds of the room
	SDL_Rect result;
	auto isWithinRoom = SDL_IntersectRect(&room->InnerBounds, &mockPlayerHotSpotBounds, &result) == SDL_TRUE ? true : false;

	/*std::stringstream ss2;
	ss2 << "Room Inner Bounds (" << room->InnerBounds.x << ", " << room->InnerBounds.y << ", w=" << room->InnerBounds.w << " h=" << room->InnerBounds.h << std::endl;
	gamelib::Logger::Get()->LogThis(ss2.str());
	
	std::stringstream ss;
	ss << "Hotspot Preview: (" << mockPlayerHotSpotBounds.x << ", " << mockPlayerHotSpotBounds.y << ", w=" << mockPlayerHotSpotBounds.w << " h=" << mockPlayerHotSpotBounds.h << std::endl;
	gamelib::Logger::Get()->LogThis(ss.str());
	
	std::stringstream ss1;	
	ss1 << "Result Preview: (" << result.x << ", " << result.y << ", w=" << result.w << " h=" << result.h << std::endl;
	gamelib::Logger::Get()->LogThis(ss1.str());

	std::stringstream ss3;
	ss3 << "IsWithinRoom: " << room->GetRoomNumber() << std::string((isWithinRoom ? "yes" : "no")) << std::endl;
	gamelib::Logger::Get()->LogThis(ss3.str());*/
	return isWithinRoom;
}


bool EdgeTowardsRoomStrategy::CanPlayerMoveUp(std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& aboveRoom, std::shared_ptr<Movement> movement)
{
	auto hasValidTargetRoom = aboveRoom != nullptr;
	auto hasBlockingWalls = (!hasValidTargetRoom || aboveRoom->HasBottomWall() && currentRoom->HasTopWall());
	auto isWithinCurrentRoom = WouldPlayerHotspotHitRoomInnerBounds(currentRoom, movement);
	auto isInNoMansLand = !isWithinCurrentRoom && (hasValidTargetRoom && !WouldPlayerHotspotHitRoomInnerBounds(aboveRoom, movement));
	
	return isWithinCurrentRoom || isInNoMansLand || (!hasBlockingWalls);
}


bool EdgeTowardsRoomStrategy::CanPlayerMoveDown(std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& bottomRoom, std::shared_ptr<Movement> movement)
{
	auto hasValidTargetRoom = bottomRoom != nullptr;
	auto hasBlockingWalls = (!hasValidTargetRoom || bottomRoom->HasTopWall()) && currentRoom->HasBottomWall();
	auto isWithinCurrentRoom = WouldPlayerHotspotHitRoomInnerBounds(currentRoom, movement);
	auto isInNoMansLand = !isWithinCurrentRoom && (hasValidTargetRoom && !WouldPlayerHotspotHitRoomInnerBounds(bottomRoom, movement));

	return isWithinCurrentRoom || isInNoMansLand || (!hasBlockingWalls);
}

bool EdgeTowardsRoomStrategy::CanPlayerMoveLeft(std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& leftRoom, std::shared_ptr<Movement> movement)
{
	auto hasValidTargetRoom = leftRoom != nullptr;
	auto hasBlockingWalls = (!hasValidTargetRoom || leftRoom->HasRightWall()) && currentRoom->HasLeftWall();
	auto isWithinCurrentRoom = WouldPlayerHotspotHitRoomInnerBounds(currentRoom, movement);
	auto isInNoMansLand = !isWithinCurrentRoom && (hasValidTargetRoom && !WouldPlayerHotspotHitRoomInnerBounds(leftRoom, movement));

	return isWithinCurrentRoom || isInNoMansLand || (!hasBlockingWalls);
}

bool EdgeTowardsRoomStrategy::CanPlayerMoveRight(std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& rightRoom, std::shared_ptr<Movement> movement)
{
	auto hasValidTargetRoom = rightRoom != nullptr;
	auto hasBlockingWalls = (!hasValidTargetRoom || rightRoom->HasLeftWall()) && currentRoom->HasRightWall();
	auto isWithinCurrentRoom = WouldPlayerHotspotHitRoomInnerBounds(currentRoom, movement);
	auto isInNoMansLand = !isWithinCurrentRoom && (hasValidTargetRoom && !WouldPlayerHotspotHitRoomInnerBounds(rightRoom, movement));

	return isWithinCurrentRoom || isInNoMansLand || (!hasBlockingWalls);
}
