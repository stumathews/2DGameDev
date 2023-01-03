#include "pch.h"
#include "Level.h"

class LevelTests : public testing::Test 
{
public:

	void SetUp() override
	{
	}

	void TearDown() override
	{
		
	}
	
};

TEST_F(LevelTests, Load)
{
	Level level("balls.xml");
	level.Load();

	EXPECT_EQ(level.Rooms.size(), 100) << "level is not the right number of rooms";
}