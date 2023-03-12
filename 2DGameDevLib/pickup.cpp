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

#include "EventNumber.h"
#include "SDLCollisionDetection.h"
#include "GameData.h"
#include "PlayerCollidedWithPickupEvent.h"

using namespace std;

namespace gamelib 
{
	void Pickup::Initialize()
	{		
		SetBounds();

		sprite = AnimatedSprite::Create(Position, dynamic_pointer_cast<SpriteAsset>(ResourceManager::Get()->GetAssetInfo(Asset->name)));
		width = sprite->Dimensions.GetWidth();
		height = sprite->Dimensions.GetHeight();
	}

	ListOfEvents Pickup::HandleEvent(const shared_ptr<Event> event, unsigned long deltaMs)
	{
		ListOfEvents generatedEvents;

		if(event->Id.PrimaryId == PlayerMovedEventTypeEventId.PrimaryId)  // NOLINT(clang-diagnostic-switch-enum)
		{			
			const auto player = GameData::Get()->GetPlayer();

			if (player->CurrentRoom->GetCurrentRoom()->GetRoomNumber() == RoomNumber)
			{
				if (SdlCollisionDetection::IsColliding(&player->Bounds, &Bounds))
				{
					generatedEvents.push_back(make_shared<Event>(FetchedPickupEventId));
					generatedEvents.push_back(make_shared<PlayerCollidedWithPickupEvent>(player, shared_from_this()));
					generatedEvents.push_back(make_shared<GameObjectEvent>(shared_from_this(),
						GameObjectEventContext::Remove));
				}
			}			
		}
		return generatedEvents;
	}

	void Pickup::Draw(SDL_Renderer* renderer)
	{
		sprite->Draw(renderer);		
	}

	void Pickup::SetBounds() { Bounds = CalculateBounds(Position, width, height); }

	void Pickup::Update(const unsigned long deltaMs)
	{
		sprite->Position.SetX(Position.GetX());
		sprite->Position.SetY(Position.GetY());

		// Progress sprite frame time
		sprite->Update(deltaMs);
	}	
}