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
	return (!aboveRoom->HasBottomWall() && !currentRoom->HasTopWall()) || !WouldHitInnerBounds(aboveRoom);
}


bool EdgeTowardsRoomStrategy::CanMoveDown(const bool& isMovingDown, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& bottomRoom)
{
	return (!bottomRoom->HasTopWall() && !currentRoom->HasBottomWall()) || !WouldHitInnerBounds(bottomRoom);
}

bool EdgeTowardsRoomStrategy::CanMoveLeft(const bool& isMovingLeft, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& leftRoom)
{
	return (!leftRoom->HasRightWall() && !currentRoom->HasLeftWall()) || !WouldHitInnerBounds(leftRoom);
}

bool EdgeTowardsRoomStrategy::CanMoveRight(const bool& isMovingRight, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& rightRoom)
{
	return (!rightRoom->HasLeftWall() && !currentRoom->HasRightWall()) || !WouldHitInnerBounds(rightRoom);
}
