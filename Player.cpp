#include "Player.h"
#include "PlayerComponent.h"
#include "PlayerMovedEvent.h"
#include "constants.h"

vector<shared_ptr<Event>> Player::ProcessEvent(const std::shared_ptr<Event> event)
{	
	// Process GameObject events
	auto createdEvents(GameObject::ProcessEvent(event));

	// Process Square events
	for(auto e : Square::ProcessEvent(event)) 
		createdEvents.push_back(e);
	
	// Process Player events
	if(event->m_eventType == PositionChangeEventType)
	{
		auto playerComponent = static_pointer_cast<PlayerComponent>(FindComponent(constants::playerComponentName));		
		auto playerMovedEvent = shared_ptr<PlayerMovedEvent>(new PlayerMovedEvent(playerComponent));

		
		playerComponent->x = GetX();
		playerComponent->y = GetY();
		playerComponent->w = GetW();
		playerComponent->h = GetH();
		playerBounds.x = playerComponent->x;
		playerBounds.y = playerComponent->y;
		playerBounds.w = playerComponent->w;
		playerBounds.h = playerComponent->h;

		createdEvents.push_back(playerMovedEvent);	
	}
	return createdEvents;
}

void Player::VDraw(SDL_Renderer* renderer)
{
  // Let the player draw itself
	SDL_RenderDrawRect(renderer, &playerBounds);
}
