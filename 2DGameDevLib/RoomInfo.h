#pragma once
#include <memory>

#include "Room.h"

class RoomInfo
{
public:

	RoomInfo(const std::shared_ptr<Room>& room);
	std::shared_ptr<Room> TheRoom;	
	int RoomIndex = 0;

	[[nodiscard]] std::shared_ptr<Room> GetTopRoom() const { return GetAdjacentRoomTo(GetCurrentRoom(), Side::Top); }
	[[nodiscard]] std::shared_ptr<Room> GetBottomRoom() const { return GetAdjacentRoomTo(GetCurrentRoom(), Side::Bottom); }
	[[nodiscard]] std::shared_ptr<Room> GetRightRoom() const { return GetAdjacentRoomTo(GetCurrentRoom(), Side::Right); }
	[[nodiscard]] std::shared_ptr<Room> GetLeftRoom() const { return GetAdjacentRoomTo(GetCurrentRoom(), Side::Left); }
	[[nodiscard]] std::shared_ptr<Room> GetCurrentRoom() const;
	static std::shared_ptr<Room> GetRoomByIndex(int index);
	static std::shared_ptr<Room> GetAdjacentRoomTo(const std::shared_ptr<Room>& room, Side side);
	void SetCurrentRoom(const std::shared_ptr<Room>& room);
};
