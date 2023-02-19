#include "pch.h"
#include "CharacterBuilder.h"

#include "Enemy.h"
#include "GameData.h"
#include "GameObjectMoveStrategy.h"
#include "SpriteAsset.h"
#include "common/constants.h"
#include "objects/GameObjectFactory.h"


std::shared_ptr<Player> CharacterBuilder::BuildPlayer(const std::string& name, const std::shared_ptr<Room>& room, const int resourceId, const std::string& nickName)
{
	// Get resource/asset
	const auto spriteAsset = std::dynamic_pointer_cast<gamelib::SpriteAsset>(gamelib::ResourceManager::Get()->GetAssetInfo(resourceId));

	// Build sprite
	const auto animatedSprite = gamelib::GameObjectFactory::Get().BuildSprite(
		name, "Player", spriteAsset, room->GetCenter(spriteAsset->Dimensions), true);

	// Build player
	auto player = std::make_shared<Player>(name, "Player", room, nickName);

	// Initialize player
	player->LoadSettings();
	player->SetMoveStrategy(std::make_shared<GameObjectMoveStrategy>(player, player->CurrentRoom));
	player->SetTag(gamelib::constants::PlayerTag);	
	player->SetSprite(animatedSprite);
	player->IntProperties["Health"] = 100;

	// We keep a reference to track of the player globally
	GameData::Get()->player = player;

	return player;
}

std::shared_ptr<Enemy> CharacterBuilder::BuildEnemy(const std::string& name, const std::shared_ptr<Room>& room,
                                                const int resourceId, gamelib::Direction startingDirection, std::shared_ptr<const Level>
                                                level)
{
	const auto spriteAsset = std::dynamic_pointer_cast<gamelib::SpriteAsset>(
		gamelib::ResourceManager::Get()->GetAssetInfo(resourceId));

	const auto positionInRoom = room->GetCenter(spriteAsset->Dimensions);

	// Build sprite
	const auto animatedSprite = gamelib::GameObjectFactory::Get().BuildSprite(
		name, "Enemy", spriteAsset, positionInRoom, true);
	
	auto enemy = std::make_shared<Enemy>(name, "Enemy", positionInRoom, true, room, animatedSprite, startingDirection, level);
	return enemy;
}


std::shared_ptr<gamelib::Pickup> CharacterBuilder::BuildPickup(const std::string& name, const std::shared_ptr<Room>& room, const int resourceId)
{
	const auto spriteAsset = std::dynamic_pointer_cast<gamelib::SpriteAsset>(
		gamelib::ResourceManager::Get()->GetAssetInfo(resourceId));

	auto pickup = std::make_shared<gamelib::Pickup>(name, "Pickup", room->GetCenter(spriteAsset->Dimensions), true,
	                                                room->GetRoomNumber(), spriteAsset);

	pickup->Initialize();
	return pickup;
}

