#include "pch.h"
#include "Room.h"

TEST(RoomTests, Basic)
{
	int number = 1;
	int x = 0;
	int y = 0;
	int width = 10;
	int height = 10;
	bool fill = false;
	Room room(number, x, y, width, height, fill);

	// Expect all walls of a room to be walled solid by default
	EXPECT_TRUE(room.IsWalled(Side::Top));
	EXPECT_TRUE(room.IsWalled(Side::Bottom));
	EXPECT_TRUE(room.IsWalled(Side::Left));
	EXPECT_TRUE(room.IsWalled(Side::Right));
	EXPECT_TRUE(room.isActive);
	EXPECT_TRUE(room.isVisible);
	EXPECT_EQ(room.GetWidth(), width);
	EXPECT_EQ(room.GetHeight(), height);
	EXPECT_EQ(room.GetX(), x);
	EXPECT_EQ(room.GetY(), y);
	
}