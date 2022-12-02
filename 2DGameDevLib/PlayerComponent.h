#pragma once
#include <objects/Component.h>
#include "Player.h"
#include "Room.h"

using namespace gamelib;

class PlayerComponent : public Component
{
public:
	PlayerComponent(std::string component_name, Player* player);

	Player* the_player;
		
	int room;
};

