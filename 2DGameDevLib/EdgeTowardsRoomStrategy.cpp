#include "EdgeTowardsRoomStrategy.h"
#include "EdgeTowardsRoomStrategy.h"
#include "EdgeTowardsRoomStrategy.h"
#include "EdgeTowardsRoomStrategy.h"
#include "pch.h"
#include "EdgeTowardsRoomStrategy.h"
#include "Player.h"
#include "Room.h"
#include "scene/SceneManager.h"

EdgeTowardsRoomStrategy::EdgeTowardsRoomStrategy(std::shared_ptr<Player> player, int edgeIncrement)
{
	this->player = player;
	this->edgeIncrement = edgeIncrement;
}

void EdgeTowardsRoomStrategy::MoveTo(std::shared_ptr<Room> room)
{
	// Edge player towards the room
	
	int move_interval = 3;

	auto gameObjects = gamelib::SceneManager::Get()->GetGameWorld().GetGameObjects();
	auto playersRoom = player->GetCurrentRoom(gameObjects);
	auto moveDown = playersRoom->GetNeighbourIndex(Side::Bottom) == room->GetRoomNumber();
	auto moveUp = playersRoom->GetNeighbourIndex(Side::Top) == room->GetRoomNumber();
	auto moveLeft = playersRoom->GetNeighbourIndex(Side::Left) == room->GetRoomNumber();
	auto moveRight = playersRoom->GetNeighbourIndex(Side::Right) == room->GetRoomNumber();

	if(moveDown)
	{
		player->y += move_interval;
	}

	if(moveUp)
	{
		player->y -= move_interval;
	}

	if(moveLeft)
	{
		player->x -= move_interval;
	}

	if(moveRight)
	{
		player->x += move_interval;
	}
	
	
	player->Update(0.0f);
}

bool EdgeTowardsRoomStrategy::CanMoveUp(const bool& isMovingUp, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& aboveRoom)
{
	return true;
}

bool EdgeTowardsRoomStrategy::CanMoveDown(const bool& isMovingDown, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& bottomRoom)
{
	return true;
}

bool EdgeTowardsRoomStrategy::CanMoveLeft(const bool& isMovingLeft, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& leftRoom)
{
	return true;
}

bool EdgeTowardsRoomStrategy::CanMoveRight(const bool& isMovingRight, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& rightRoom)
{
	return true;
}
