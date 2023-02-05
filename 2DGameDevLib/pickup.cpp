#include "pch.h"
#include "Pickup.h"
#include "events/PlayerMovedEvent.h"
#include <events/GameObjectEvent.h>
#include <common/Common.h>
#include <scene/SceneManager.h>
#include "player.h"
#include <memory>
#include <SpriteAsset.h>
#include <common/aliases.h>
#include "SDLCollisionDetection.h"
#include "GameData.h"

using namespace std;

namespace gamelib 
{
	void Pickup::Initialize()
	{		
		SetBounds();

		_sprite = AnimatedSprite::Create(Position, dynamic_pointer_cast<SpriteAsset>(ResourceManager::Get()->GetAssetInfo(Asset->name)));
		width = _sprite->Dimensions.GetWidth();
		height = _sprite->Dimensions.GetHeight();
		CalculateBounds(Position, width, height);
	}

	ListOfEvents Pickup::HandleEvent(const shared_ptr<Event> event, unsigned long deltaMs)
	{
		ListOfEvents generatedEvents;

		switch(event->Type)  // NOLINT(clang-diagnostic-switch-enum)
		{
			case EventType::PlayerMovedEventType:	
			{
				const auto player = GameData::Get()->GetPlayer();

				if (player->GetCurrentRoom()->GetRoomNumber() == RoomNumber)
				{
					if (SdlCollisionDetection::IsColliding(&player->Bounds, &Bounds))
					{
						generatedEvents.push_back(make_shared<Event>(EventType::FetchedPickup));
						generatedEvents.push_back(make_shared<GameObjectEvent>(shared_from_this(),
							GameObjectEventContext::Remove));
					}
				}
			}
			break;
			default: /* Do Nothing */;
		}
		return generatedEvents;
	}

	void Pickup::Draw(SDL_Renderer* renderer)
	{
		_sprite->Draw(renderer);		
	}

	void Pickup::SetBounds() { Bounds = { Position.GetX(), Position.GetY(), width, height }; }

	void Pickup::Update(const float deltaMs)
	{
		_sprite->Position.SetX(Position.GetX());
		_sprite->Position.SetY(Position.GetY());

		// Progress sprite frame time
		_sprite->Update(deltaMs);
	}	
}