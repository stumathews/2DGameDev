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
	EXPECT_TRUE(room.IsActive);
	EXPECT_TRUE(room.IsVisible);
	EXPECT_EQ(room.GetWidth(), width);
	EXPECT_EQ(room.GetHeight(), height);
	EXPECT_EQ(room.GetX(), x);
	EXPECT_EQ(room.GetY(), y);
	EXPECT_FALSE(room.HasGraphic());
	EXPECT_EQ(room.GetGameObjectType(), gamelib::GameObjectType::Room);
	EXPECT_EQ(room.GetGraphic(), nullptr);
	EXPECT_FALSE(room.HasColourKey());
	EXPECT_EQ(room.GetColourKey().r, 0);
	EXPECT_EQ(room.GetColourKey().b, 0);
	EXPECT_EQ(room.GetColourKey().g, 0);	
}

TEST(RoomTests, RemoveWalls)
{
	int number = 1;
	int x = 0;
	int y = 0;
	int width = 10;
	int height = 10;
	bool fill = false;
	Room room(number, x, y, width, height, fill);

	EXPECT_TRUE(room.IsWalled(Side::Bottom));
	EXPECT_TRUE(room.IsWalled(Side::Top));	
	EXPECT_TRUE(room.IsWalled(Side::Left));
	EXPECT_TRUE(room.IsWalled(Side::Right));

	// Remove Wall
	room.RemoveWall(Side::Bottom);
	EXPECT_FALSE(room.IsWalled(Side::Bottom));

	// Remove Wall
	room.RemoveWall(Side::Top);
	EXPECT_FALSE(room.IsWalled(Side::Top));

	// Remove Wall
	room.RemoveWall(Side::Left);
	EXPECT_FALSE(room.IsWalled(Side::Left));

	// Remove Wall
	room.RemoveWall(Side::Right);
	EXPECT_FALSE(room.IsWalled(Side::Right));
}

TEST(RoomTests, AddWalls)
{
	int number = 1;
	int x = 0;
	int y = 0;
	int width = 10;
	int height = 10;
	bool fill = false;
	Room room(number, x, y, width, height, fill);

	EXPECT_TRUE(room.IsWalled(Side::Bottom));
	EXPECT_TRUE(room.IsWalled(Side::Top));
	EXPECT_TRUE(room.IsWalled(Side::Left));
	EXPECT_TRUE(room.IsWalled(Side::Right));

	// Remove Wall
	room.RemoveWall(Side::Bottom);
	EXPECT_FALSE(room.IsWalled(Side::Bottom));

	// Remove Wall
	room.RemoveWall(Side::Top);
	EXPECT_FALSE(room.IsWalled(Side::Top));

	// Remove Wall
	room.RemoveWall(Side::Left);
	EXPECT_FALSE(room.IsWalled(Side::Left));

	// Remove Wall
	room.RemoveWall(Side::Right);
	EXPECT_FALSE(room.IsWalled(Side::Right));

	room.AddWall(Side::Top);
	room.AddWall(Side::Right);
	room.AddWall(Side::Bottom);
	room.AddWall(Side::Left);

	EXPECT_TRUE(room.IsWalled(Side::Bottom));
	EXPECT_TRUE(room.IsWalled(Side::Top));
	EXPECT_TRUE(room.IsWalled(Side::Left));
	EXPECT_TRUE(room.IsWalled(Side::Right));
}