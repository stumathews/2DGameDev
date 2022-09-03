#include "pch.h"
#include "Level.h"

TEST(LevelTests, Load)
{
	Level level("balls.xml");
	level.Load();

	EXPECT_EQ(level.Rooms.size(), 100) << "level is not the right number of rooms";
}