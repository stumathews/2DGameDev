#include "pch.h"
#include "PlayerComponent.h"

PlayerComponent::PlayerComponent(std::string component_name, Player* player)
	: Component(component_name), the_player(player), room(0)
{
	// init members only
}
