#pragma once
#include "Event.h"
#include "PlayerComponent.h"
#include <memory>
class PlayerMovedEvent : public Event
{
public:
	PlayerMovedEvent(shared_ptr<PlayerComponent> playerComponent) : Event(PlayerMovedEventType), m_PlayerComponent(playerComponent)
	{
		
	}
	shared_ptr<PlayerComponent> GetPlayerComponent() { return m_PlayerComponent;}
private:
	shared_ptr<PlayerComponent> m_PlayerComponent;

};

