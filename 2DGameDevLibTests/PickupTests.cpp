
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-more-matchers.h>
#include <gtest/gtest.h>

#include "CharacterBuilder.h"
#include "GameData.h"
#include "Room.h"
#include "resource/ResourceManager.h"

class PickupTests : public testing::Test 
{
 protected:

	void SetUp() override
	{
		gamelib::ResourceManager::Get()->Initialize("Resources.xml");
		level = std::make_shared<Level>();
		room = std::make_shared<Room>("MyRoom", "Room", 0, 0, 0, 100, 100);
		spriteAsset = std::dynamic_pointer_cast<gamelib::SpriteAsset>(
			gamelib::ResourceManager::Get()->GetAssetInfo(pickupAssetId));
		
		pickup = std::make_shared<gamelib::Pickup>(
			pickupName, "Pickup", room->GetCenter(spriteAsset->Dimensions), true,
			room->GetRoomNumber(), spriteAsset);
	}

	void TearDown() override
	{
	}

	std::string pickupName {"MyPickup01"};
	std::shared_ptr<Level> level;
	std::shared_ptr<Room> room;
	std::shared_ptr<gamelib::Pickup> pickup;
	const int pickupAssetId {19};
	std::shared_ptr<gamelib::SpriteAsset> spriteAsset;
};

TEST_F(PickupTests, Construction)
{
	EXPECT_EQ(pickup->GetGameObjectType(), gamelib::GameObjectType::pickup);
	EXPECT_EQ(pickup->GetSubscriberName(), pickupName);
	EXPECT_EQ(pickup->GetName(), pickupName);
	EXPECT_EQ(pickup->RoomNumber, room->GetRoomNumber());
	EXPECT_THAT(pickup->IntProperties, testing::IsEmpty());
	EXPECT_THAT(pickup->Asset->Uid, testing::Eq(pickupAssetId));
}

TEST_F(PickupTests, Initialize)
{
	EXPECT_NO_THROW(pickup->Initialize());
}