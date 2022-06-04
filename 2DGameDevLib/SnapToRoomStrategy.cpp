#include "pch.h"
#include "SnapToRoomStrategy.h"
using namespace std;

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
	player->Update(10.f);
}

bool SnapToRoomStrategy::CanMoveUp(const bool& isMovingUp, shared_ptr<Room>& currentRoom, shared_ptr<Room>& aboveRoom)
{
	return isMovingUp && !currentRoom->IsWalled(Side::Top) && !aboveRoom->IsWalled(Side::Bottom);
}

bool SnapToRoomStrategy::CanMoveDown(const bool& isMovingDown, shared_ptr<Room>& currentRoom, shared_ptr<Room>& bottomRoom)
{
	return isMovingDown && !currentRoom->IsWalled(Side::Bottom) && !bottomRoom->IsWalled(Side::Top);
}

bool SnapToRoomStrategy::CanMoveLeft(const bool& isMovingLeft, shared_ptr<Room>& currentRoom, shared_ptr<Room>& leftRoom)
{
	return isMovingLeft && !currentRoom->IsWalled(Side::Left) && !leftRoom->IsWalled(Side::Right);
}

bool SnapToRoomStrategy::CanMoveRight(const bool& isMovingRight, shared_ptr<Room>& currentRoom, shared_ptr<Room>& rightRoom)
{
	return isMovingRight && !currentRoom->IsWalled(Side::Right) && !rightRoom->IsWalled(Side::Left);
}
