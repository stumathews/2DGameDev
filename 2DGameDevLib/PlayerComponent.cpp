#include "pch.h"
#include "PlayerComponent.h"

PlayerComponent::PlayerComponent(const std::string& component_name, Player* player)
	: Component(component_name), ThePlayer(player), Room(0)
{
	// init members only
}
