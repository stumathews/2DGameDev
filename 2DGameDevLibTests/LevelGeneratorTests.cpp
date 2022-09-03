#include "pch.h"
#include "RoomGenerator.h"

namespace LevelGenerator
{
	TEST(LevelGeneratorTests, Generate)
	{
		RoomGenerator levelGenerator(800, 600, 10, 10, true);
		auto result = levelGenerator.Generate();
		EXPECT_EQ(result.size(), 100) << "Generated level is not the right number of rooms";
		EXPECT_EQ(result[0]->GetTag(), "0") << "First room number tag not expected";
		EXPECT_EQ(result[99]->GetTag(), "99") << "First room number tag not expected";
	}
}