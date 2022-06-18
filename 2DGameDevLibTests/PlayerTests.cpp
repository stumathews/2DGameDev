#include "pch.h"
#include <Player.h>

TEST(PlayerTests, Basic)
{
	Player player(0,0,0,0);
	EXPECT_EQ(player.Position.GetX(), 0);
	EXPECT_EQ(player.Position.GetY(), 0);
	EXPECT_EQ(player.Bounds.w, 0);
	EXPECT_EQ(player.Bounds.h, 0);
	EXPECT_EQ(player.Bounds.x, 0);
	EXPECT_EQ(player.Bounds.y, 0);
	EXPECT_EQ(player.Components.Count(), 0);
	EXPECT_EQ(player.GetGameObjectType(), gamelib::GameObjectType::Player);		
}

TEST(PlayerTests, HandleEvents)
{
	Player player(0, 0, 0, 0);
}