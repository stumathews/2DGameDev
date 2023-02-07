#include "pch.h"
#include "CharacterBuilder.h"
#include "GameData.h"
#include "Npc.h"
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

	// We keep a reference to track of the player globally
	GameData::Get()->player = player;

	return player;
}

std::shared_ptr<Npc> CharacterBuilder::BuildNpc(const std::string& name, const std::shared_ptr<Room>& room,
                                                const int resourceId)
{
	const auto spriteAsset = std::dynamic_pointer_cast<gamelib::SpriteAsset>(
		gamelib::ResourceManager::Get()->GetAssetInfo(resourceId));

	const auto positionInRoom = room->GetCenter(spriteAsset->Dimensions);

	// Build sprite
	const auto animatedSprite = gamelib::GameObjectFactory::Get().BuildSprite(
		name, "Npc", spriteAsset, positionInRoom, true);

	// Build player
	auto npc = std::make_shared<Npc>(name, "Npc", positionInRoom, true, room, animatedSprite);
	npc->Initialize();
	return npc;
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

