#include "pch.h"
#include "Player.h"
#include "Room.h"
#include "scene/SceneManager.h"
#include "EdgeTowardsRoomStrategy.h"

EdgeTowardsRoomStrategy::EdgeTowardsRoomStrategy(std::shared_ptr<Player> player, int edgeIncrement)
{
	this->player = player;
	this->edgeIncrement = edgeIncrement;
}

gamelib::coordinate<int> EdgeTowardsRoomStrategy::CalculatePlayerMoveTo(std::shared_ptr<Room> room)
{
	int move_interval = 3;

	auto gameObjects = gamelib::SceneManager::Get()->GetGameWorld().GetGameObjects();
	auto playersRoom = player->GetCurrentRoom(gameObjects);
	auto moveDown = playersRoom->GetNeighbourIndex(Side::Bottom) == room->GetRoomNumber();
	auto moveUp = playersRoom->GetNeighbourIndex(Side::Top) == room->GetRoomNumber();
	auto moveLeft = playersRoom->GetNeighbourIndex(Side::Left) == room->GetRoomNumber();
	auto moveRight = playersRoom->GetNeighbourIndex(Side::Right) == room->GetRoomNumber();
		
	int resulting_x;	
	int resulting_y; 
	
	resulting_y = player->y;
	resulting_x = player->x;
	if(moveDown)
	{
		resulting_y += move_interval;
	}

	if(moveUp)
	{
		resulting_y -= move_interval;
	}

	if(moveLeft)
	{
		resulting_x -= move_interval;
	}

	if(moveRight)
	{
		resulting_x += move_interval;
	}

	return gamelib::coordinate<int>(resulting_x, resulting_y);
}

void EdgeTowardsRoomStrategy::MoveTo(std::shared_ptr<Room> room)
{
	// Edge player towards the room

	auto resultingMove = CalculatePlayerMoveTo(room);

	player->x = resultingMove.GetX();
	player->y = resultingMove.GetY();
	
	
	player->Update(0.0f);
}


bool EdgeTowardsRoomStrategy::WouldHitInnerBounds(std::shared_ptr<Room>& room)
{
	auto mockPlayerPosition = CalculatePlayerMoveTo(room);
	auto mockPlayerHotSpot = player->CalculateHotspot(mockPlayerPosition.GetX(), mockPlayerPosition.GetY());
	auto mockPlayerHotSpotBounds = SDL_Rect { mockPlayerHotSpot.GetX(), mockPlayerHotSpot.GetY(), 1, 1 };
	SDL_Rect result;
	auto wouldItHitIt = SDL_IntersectRect(&room->InnerBounds, &mockPlayerHotSpotBounds, &result);
	return wouldItHitIt;
}

bool EdgeTowardsRoomStrategy::CanMoveUp(const bool& isMovingUp, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& aboveRoom)
{
	auto isTerminalRoom = aboveRoom->GetRoomNumber() == currentRoom->GetRoomNumber();
	auto hasNoBlockingWalls =!aboveRoom->HasBottomWall() && !currentRoom->HasTopWall();

	if(isTerminalRoom)
	{
		return WouldHitInnerBounds(currentRoom);
	}

	return hasNoBlockingWalls || !WouldHitInnerBounds(aboveRoom);
}


bool EdgeTowardsRoomStrategy::CanMoveDown(const bool& isMovingDown, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& bottomRoom)
{
	auto isTerminalRoom = bottomRoom->GetRoomNumber() == currentRoom->GetRoomNumber();
	auto hasNoBlockingWalls = !bottomRoom->HasTopWall() && !currentRoom->HasBottomWall();
	if(isTerminalRoom)
	{
		return WouldHitInnerBounds(currentRoom);
	}

	return hasNoBlockingWalls || !WouldHitInnerBounds(bottomRoom);
}

bool EdgeTowardsRoomStrategy::CanMoveLeft(const bool& isMovingLeft, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& leftRoom)
{
	auto isTerminalRoom = leftRoom->GetRoomNumber() == currentRoom->GetRoomNumber();
	auto hasNoBlockingWalls = !leftRoom->HasRightWall() && !currentRoom->HasLeftWall();
	if(isTerminalRoom)
	{
		return WouldHitInnerBounds(currentRoom);
	}
	return hasNoBlockingWalls || !WouldHitInnerBounds(leftRoom);
}

bool EdgeTowardsRoomStrategy::CanMoveRight(const bool& isMovingRight, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& rightRoom)
{
	auto isTerminalRoom = rightRoom->GetRoomNumber() == currentRoom->GetRoomNumber();
	auto hasNoBlockingWalls = !rightRoom->HasLeftWall() && !currentRoom->HasRightWall();
	if(isTerminalRoom)
	{
		return WouldHitInnerBounds(currentRoom);
	}
	return hasNoBlockingWalls || !WouldHitInnerBounds(rightRoom);
}
