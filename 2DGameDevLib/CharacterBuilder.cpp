#include "pch.h"
#include "CharacterBuilder.h"
#include "Enemy.h"
#include "GameData.h"
#include "GameObjectMoveStrategy.h"
#include "asset/SpriteAsset.h"
#include "common/constants.h"
#include "events/PlayerMovedEvent.h"
#include "objects/GameObjectFactory.h"
#include <utils/Utils.h>

using namespace gamelib;

std::shared_ptr<Player> CharacterBuilder::BuildPlayer(const std::string& name, const std::shared_ptr<Room>& room,
                                                      const int resourceId, const std::string& nickName)
{
	// Get resource/asset from resource manager
	const auto spriteAsset = To<SpriteAsset>(ResourceManager::Get()->GetAssetInfo(resourceId));

	// Build sprite
	const auto animatedSprite = GameObjectFactory::BuildSprite(
		name,
		"Player",
		spriteAsset,
		room->GetCenter(spriteAsset->Dimensions),
		true);

	// Build player
	auto player = std::make_shared<Player>(
		name,
		"Player", 
		room,
		spriteAsset->Dimensions, 
		nickName);

	// Initialize player
	player->LoadSettings();
	player->SetMoveStrategy(std::make_shared<GameObjectMoveStrategy>(player, player->CurrentRoom));
	player->SetTag(gamelib::PlayerTag);
	player->SetSprite(animatedSprite);
	player->IntProperties["Health"] = 100;

	// We keep a reference to track of the player globally
	GameData::Get()->player = player;

	return player;
}

std::shared_ptr<Enemy> CharacterBuilder::BuildEnemy(const std::string& name, const std::shared_ptr<Room>& room,
                                                    const int spriteResourceId, Direction startingDirection,
                                                    const std::shared_ptr<const Level>& level)
{
	const auto spriteAsset = To<SpriteAsset>(ResourceManager::Get()->GetAssetInfo(spriteResourceId));

	const auto positionInRoom = room->GetCenter(spriteAsset->Dimensions);

	// Build sprite
	const auto animatedSprite = GameObjectFactory::BuildSprite(
		name, 
	"Enemy", 
		spriteAsset, 
		positionInRoom, 
		true);

	auto enemy = std::make_shared<Enemy>(name,
		"Enemy",
		positionInRoom,
		true,
		room,
		animatedSprite,
		startingDirection,
		level);

	return enemy;
}


std::shared_ptr<Pickup> CharacterBuilder::BuildPickup(const std::string& name,
                                                      const std::shared_ptr<Room>& room, const int resourceId)
{
	const auto spriteAsset = To<SpriteAsset>(ResourceManager::Get()->GetAssetInfo(resourceId));

	auto pickup = std::make_shared<Pickup>(name, "Pickup", 
		room->GetCenter(spriteAsset->Dimensions),
		true,
		room->GetRoomNumber(), 
		spriteAsset);

	pickup->Initialize();
	pickup->LoadSettings();
	pickup->SubscribeToEvent(PlayerMovedEventTypeEventId);
	return pickup;
}
