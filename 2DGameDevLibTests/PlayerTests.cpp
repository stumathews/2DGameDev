#include "pch.h"
#include <Player.h>

TEST(PlayerTests, Basic)
{
	Player player(0,0,0,0);
	EXPECT_EQ(player.x, 0);
	EXPECT_EQ(player.y, 0);
	EXPECT_EQ(player.bounds.w, 0);
	EXPECT_EQ(player.bounds.h, 0);
	EXPECT_EQ(player.bounds.x, 0);
	EXPECT_EQ(player.bounds.y, 0);
	EXPECT_EQ(player.components.Count(), 0);
	EXPECT_EQ(player.GetGameObjectType(), gamelib::GameObjectType::Player);		
}

TEST(PlayerTests, HandleEvents)
{
	Player player(0, 0, 0, 0);
}