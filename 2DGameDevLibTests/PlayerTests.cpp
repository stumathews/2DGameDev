#include "pch.h"
#include <Player.h>

TEST(PlayerTests, Basic)
{
	Player _player(gamelib::coordinate<int>(0,0), 0, 0, "player1");
	EXPECT_EQ(_player.Position.GetX(), 0);
	EXPECT_EQ(_player.Position.GetY(), 0);
	EXPECT_EQ(_player.Bounds.w, 0);
	EXPECT_EQ(_player.Bounds.h, 0);
	EXPECT_EQ(_player.Bounds.x, 0);
	EXPECT_EQ(_player.Bounds.y, 0);
	EXPECT_EQ(_player.Components.Count(), 0);
	EXPECT_EQ(_player.GetGameObjectType(), gamelib::GameObjectType::Player);		
}

TEST(PlayerTests, HandleEvents)
{
	Player _player(gamelib::coordinate<int>(0, 0), 0, 0, "player1");
}