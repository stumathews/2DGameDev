#include "pch.h"
#include <gtest/gtest.h>
#include "gmock/gmock.h"
#include "GameDataManager.h"
#include "Player.h"
#include <objects/GameObject.h>
#include <SDL.h>
#include <memory>

#include "GameObjectEventFactory.h"
#include "events/AddGameObjectToCurrentSceneEvent.h"

using namespace std;

class MockPlayer : public Player
{
public:
	MockPlayer(const std::string& name, const std::string& type, const gamelib::Coordinate<int>& position, int width,
		int height, const std::string& identifier)
		: Player(name, type, position, width, height, identifier)
	{
	}

	MockPlayer(const std::string& name, const std::string& type, const std::shared_ptr<Room>& playerRoom,
		int playerWidth, int playerHeight, const std::string& identifier)
		: Player(name, type, playerRoom, playerWidth, playerHeight, identifier)
	{
	}
	
  //MOCK_METHOD(void, Update, (float), (override));
  MOCK_METHOD(void, Draw, (SDL_Renderer*), (override));
  MOCK_METHOD(gamelib::GameObjectType, GetGameObjectType, (), (override));
  MOCK_METHOD(std::string, GetName,(), (override));
  MOCK_METHOD(void, LoadSettings,(), (override));
};

class GameDataManagerTests : public testing::Test 
{
public:

	void SetUp() override
	{
		Subject = GameDataManager::Get();
		Subject->Initialize();

		Player = CreateMockPlayer();
	}

	void TearDown() override
	{
		delete Subject; // Allow destroying the singleton directly
		delete gamelib::EventManager::Get();
	}
	
	GameDataManager* Subject = nullptr;

	const std::string MockPlayerName {"MockPlayer"};
	const std::string PlayerType {"Player"};
	std::shared_ptr<MockPlayer> Player;

private:
	std::shared_ptr<MockPlayer> CreateMockPlayer() const
	{		
		auto room = std::make_shared<Room>(MockPlayerName,"Room",0,0,0,0,0, false);
		return make_shared<MockPlayer>(MockPlayerName, PlayerType, room, 0, 0, MockPlayerName);
	}
};

TEST_F(GameDataManagerTests, Adds_GameObject)
{
	// Put expectations in place that dependencies will be called...
	EXPECT_CALL(*Player, GetGameObjectType()).Times(testing::AtLeast(1)).WillRepeatedly(
		testing::Return(gamelib::GameObjectType::GameDefined));

	// When receiving an event to add game object to scene
	Subject->HandleEvent(std::dynamic_pointer_cast<gamelib::Event>(gamelib::EventFactory::CreateAddToSceneEvent(Player)), 0);

	const auto gameObjects = Subject->GameData()->GameObjects;
	const auto gameObject = gameObjects[0].lock();

	// Ensure its added to the game data
	EXPECT_EQ(gameObjects.size(), 1) << "Expected 1 game object";

	// Ensure that is the expected game object added.
	EXPECT_EQ(gameObject->Name, MockPlayerName);
	EXPECT_EQ(gameObject->Type, PlayerType);	
}

TEST_F(GameDataManagerTests, Deletes_GameObject)
{
	// Put expectations in place that dependencies will be called...
	EXPECT_CALL(*Player, GetGameObjectType()).Times(testing::AtLeast(1));

	// When receiving an event to add game object to scene, and then getting event to remove it...
	Subject->HandleEvent(std::dynamic_pointer_cast<gamelib::Event>(GameObjectEventFactory::MakeAddGameObjectToSceneEvent(Player)), 0);
	Subject->HandleEvent(std::dynamic_pointer_cast<gamelib::Event>(GameObjectEventFactory::MakeRemoveObjectEvent(Player)), 0);
	
	// Ensure its removed from the game data
	EXPECT_EQ(Subject->GameData()->GameObjects.size(), 0) << "Expected 0 game object";
}

TEST_F(GameDataManagerTests, Subscriber_Name_Is_Correct)
{
	EXPECT_STREQ(Subject->GetSubscriberName().c_str(), "GameDataManager") << "Unexpected Subscriber name";
}