#include "pickup.h"
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
#include <events/UpdateAllGameObjectsEvent.h>
#include "SDLCollisionDetection.h"
#include "GameData.h"

using namespace std;

namespace gamelib 
{
	void Pickup::Initialize()
	{		
		SetBounds();

		_sprite = AnimatedSprite::Create(Position, 
			dynamic_pointer_cast<SpriteAsset>(ResourceManager::Get()->GetAssetInfo(stringProperties["assetName"])));
	}

	gamelib::ListOfEvents Pickup::HandleEvent(shared_ptr<gamelib::Event> event, unsigned long deltaMs)
	{
		gamelib::ListOfEvents generated_events;

		switch(event->type)
		{
			case gamelib::EventType::PlayerMovedEventType:	
			{
				const auto player = GameData::Get()->GetPlayer();

				if (player->GetCurrentRoom()->GetRoomNumber() == RoomNumber)
				{
					if (SDLCollisionDetection::IsColliding(&player->Bounds, &Bounds))
					{
						generated_events.push_back(make_shared<gamelib::Event>(gamelib::EventType::FetchedPickup));
						generated_events.push_back(make_shared<gamelib::GameObjectEvent>(shared_from_this(), gamelib::GameObjectEventContext::Remove));
					}
				}
			}
			break;
		}
		return generated_events;
	}

	void Pickup::Draw(SDL_Renderer* renderer)
	{
		_sprite->Draw(renderer);		
	}

	void Pickup::SetBounds() { Bounds = { Position.GetX(), Position.GetY(), width, height }; }

	void Pickup::Update(float deltaMs)
	{
		_sprite->Position.SetX(Position.GetX());
		_sprite->Position.SetY(Position.GetY());

		// Progress sprite frame time
		_sprite->Update(deltaMs);
	}

	
}