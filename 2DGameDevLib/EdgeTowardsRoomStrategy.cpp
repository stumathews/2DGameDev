#include "pch.h"
#include "EdgeTowardsRoomStrategy.h"
#include "Player.h"
#include "Room.h"
EdgeTowardsRoomStrategy::EdgeTowardsRoomStrategy(std::shared_ptr<Player> player, int edgeIncrement)
{
	this->player = player;
	this->edgeIncrement = edgeIncrement;
}

void EdgeTowardsRoomStrategy::MoveTo(std::shared_ptr<Room> room)
{
	// Edge towards the room
	
}
