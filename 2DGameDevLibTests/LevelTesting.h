#pragma once
#include <gtest/gtest.h>
#include "Level.h"
#include "resource/ResourceManager.h"

class LevelTesting : public testing::Test 
{
public:

	void SetUp() override
	{
		TheLevel = std::make_shared<Level>("Level1.xml");
	}

	void TearDown() override
	{
		
	}

	std::shared_ptr<Level> TheLevel = nullptr;
	
};

TEST_F(LevelTesting, Test_LoadsOK)
{
	
	//TheLevel->Load();

	//EXPECT_EQ(TheLevel->Rooms.size(), 100) << "level is not the right number of rooms";
}