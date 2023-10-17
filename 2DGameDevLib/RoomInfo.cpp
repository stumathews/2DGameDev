#include "pch.h"
#include "RoomInfo.h"
#include "GameData.h"
#include "Room.h"

RoomInfo::RoomInfo(const std::shared_ptr<Room>& room)
{
	TheRoom = room;
	RoomIndex = room->GetRoomNumber();
}

std::shared_ptr<Room> RoomInfo::GetCurrentRoom() const
{
	return GameData::Get()->GetRoomByIndex(RoomIndex);
}

std::shared_ptr<Room> RoomInfo::GetRoomByIndex(const int index)
{
	return GameData::Get()->GetRoomByIndex(index);
}

std::shared_ptr<Room> RoomInfo::GetAdjacentRoomTo(const std::shared_ptr<Room>& room, const gamelib::Side side)
{
	if (!room) { return nullptr; }
	return GameData::Get()->GetRoomByIndex(room->GetNeighbourIndex(side));
}

void RoomInfo::SetCurrentRoom(const std::shared_ptr<Room>& room)
{
	RoomIndex = room->GetRoomNumber();
	TheRoom = room;
}
