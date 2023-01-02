#pragma once
#include <objects/Component.h>
#include "Player.h"
#include "Room.h"

class PlayerComponent : public gamelib::Component
{
public:
	PlayerComponent(const std::string& component_name, Player* player);

	Player* ThePlayer;
		
	int Room;
};

