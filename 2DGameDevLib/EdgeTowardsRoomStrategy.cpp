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
	// Edge player towards the room
	
	int move_interval = 1;
	
	int delta_y = room->GetHotspot().GetY() - player->GetHotspot().GetY();
	int delta_x = room->GetHotspot().GetX() - player->GetHotspot().GetX();
	
	if(delta_y != 0)
		player->y = delta_y > 0 ? player->y + move_interval : player->y - move_interval;
	
	if(delta_x != 0)
		player->x = delta_x > 0 ? player->x + move_interval : player->x - move_interval;	

	if(delta_y == 0)
	{
		// horizontal movement
		player->x = delta_x > 0 ? player->x + move_interval : player->x - move_interval;
	}

	if(delta_x == 0)
	{
		// vertical movement
		player->y = delta_y > 0 ? player->y + move_interval : player->y - move_interval;
	}
	
	player->Update();
}
