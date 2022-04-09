#include "pch.h"
#include "SnapToRoomStrategy.h"

SnapToRoomStrategy::SnapToRoomStrategy(std::shared_ptr<Player> player)
{
	this->player = player;
}

void SnapToRoomStrategy::MoveTo(std::shared_ptr<Room> room)
{
	CenterPlayerInRoom(room);
}

void SnapToRoomStrategy::CenterPlayerInRoom(std::shared_ptr<Room> room)
{
	auto const room_x_mid = room->GetX() + (room->GetWidth() / 2);
	auto const room_y_mid = room->GetY() + (room->GetHeight() / 2);
	auto const x = room_x_mid - player->GetWidth() / 2;
	auto const y = room_y_mid - player->GetHeight() / 2;
	auto coords = gamelib::coordinate<int>(x, y);
	player->x = coords.GetX();
	player->y = coords.GetY();
	player->Update();
}
