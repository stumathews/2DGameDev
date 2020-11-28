#pragma once
#include "Event.h"
#include "PlayerComponent.h"
#include <memory>
#include "Player.h"
class PlayerMovedEvent : public event
{
public:
	PlayerMovedEvent(shared_ptr<PlayerComponent> playerComponent) 
		: event(PlayerMovedEventType), m_PlayerComponent(playerComponent) { }
	shared_ptr<PlayerComponent> GetPlayerComponent() { return m_PlayerComponent; }

private:
	shared_ptr<PlayerComponent> m_PlayerComponent;
};

