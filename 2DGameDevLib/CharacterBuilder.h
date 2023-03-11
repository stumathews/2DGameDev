#pragma once
#include <memory>

#include "Enemy.h"
#include "Pickup.h"
#include "Player.h"
#include "Room.h"

#include "resource/ResourceManager.h"


namespace gamelib {
class GameObject;
}


class CharacterBuilder
{
public:
	static [[nodiscard]] std::shared_ptr<Player> BuildPlayer(const std::string& name, const std::shared_ptr<Room>& room,
	                                            int resourceId, const std::string& nickName);
	static [[nodiscard]] std::shared_ptr<gamelib::Pickup> BuildPickup(const std::string& name,
	                                                                  const std::shared_ptr<Room>& room,
	                                                                  int resourceId);
	static std::shared_ptr<Enemy> BuildEnemy(const std::string& name, const std::shared_ptr<Room>& room, const int spriteResourceId,
	                                         gamelib::Direction startingDirection, const std::shared_ptr<const Level>&
	                                         level);
	
};

