#include "pch.h"
#include "PlayerComponent.h"

PlayerComponent::PlayerComponent(std::string component_name, Player* _player)
	: Component(component_name), the_player(_player), room(0)
{
	// init members only
}
