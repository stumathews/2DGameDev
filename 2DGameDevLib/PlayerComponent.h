#pragma once
#include <character/Component.h>
#include "Player.h"
#include "Room.h"

class PlayerComponent : public gamelib::Component
{
public:
	PlayerComponent(const std::string& componentName, Player* player);

	Player* ThePlayer;

	int Room;
};
