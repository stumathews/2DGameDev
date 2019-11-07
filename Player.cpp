#include "Player.h"
#include "PlayerComponent.h"
#include "PlayerMovedEvent.h"

void Player::ProcessEvent(std::shared_ptr<Event> event)
{
	GameObject::ProcessEvent(event); // This will handle basic events like responding to position changed events and updating player's x,y accordingly
	
	/* Tell people that we've moved */
	if(event->m_eventType == PositionChangeEventType)
	{
		auto playerComponent = static_pointer_cast<PlayerComponent>(FindComponent("PlayerDetails"));		
		auto playerMovedEvent = shared_ptr<PlayerMovedEvent>(new PlayerMovedEvent(playerComponent));

		playerComponent->x = GetX();
		playerComponent->y = GetY();
		playerComponent->w = GetW();
		playerComponent->h = GetH();

		RaiseEvent(playerMovedEvent);		
	}
}

void Player::VDraw(SDL_Renderer* renderer)
{
	Square::VDraw(renderer);
}

void Player::VDoLogic()
{
	
}
