#include "pch.h"
#include "Pickup.h"
#include "events/PlayerMovedEvent.h"
#include <common/Common.h>
#include <scene/SceneManager.h>
#include "player.h"
#include <memory>
#include <asset/SpriteAsset.h>
#include "EventNumber.h"
#include "SDLCollisionDetection.h"
#include "GameData.h"
#include "GameObjectEventFactory.h"
#include "PlayerCollidedWithPickupEvent.h"
#include "events/EventFactory.h"
#include "utils/Utils.h"

using namespace std;

namespace gamelib
{
	void Pickup::Initialize()
	{
		SetBounds();

		sprite = AnimatedSprite::Create(
			Position, 
			To<SpriteAsset>(ResourceManager::Get()->GetAssetInfo(Asset->Name)));
		width = sprite->Dimensions.GetWidth();
		height = sprite->Dimensions.GetHeight();
	}

	Pickup::Pickup(const std::string name, const std::string type, const int x, const int y, const int width,
		const int height, const bool visible, const int inRoomNumber)
		: DrawableGameObject(name, type, Coordinate(x, y), visible)
	{
		this->IsVisible = visible;
		this->width = width;
		this->height = height;
		this->RoomNumber = inRoomNumber;
	}

	Pickup::Pickup(const std::string name, const std::string type, const Coordinate<int> startingPoint,
	               const bool visible, const int inRoomNumber, const std::shared_ptr<SpriteAsset> asset)
		: DrawableGameObject(name, type, Coordinate(startingPoint.GetX(), startingPoint.GetY()),
	                                                                                                                         visible)
	{
		this->IsVisible = visible;
		this->Asset = asset;
		this->width = asset->Dimensions.GetWidth();
		this->height = asset->Dimensions.GetHeight();
		this->RoomNumber = inRoomNumber;
	}

	Pickup::Pickup(const bool visible): DrawableGameObject(0, 0, visible)
	{
		this->IsVisible = visible;
		this->width = 0;
		this->height = 0;
		this->RoomNumber = 0;
	}

	GameObjectType Pickup::GetGameObjectType()
	{
		return GameObjectType::Pickup;
	}

	std::string Pickup::GetSubscriberName()
	{
		return Name;
	}

	std::string Pickup::GetName()
	{
		return Name;
	}

	ListOfEvents Pickup::HandleEvent(const shared_ptr<Event> event, unsigned long deltaMs)
	{
		ListOfEvents generatedEvents;

		if (event->Id.PrimaryId == PlayerMovedEventTypeEventId.PrimaryId)
		{
			const auto player = GameData::Get()->GetPlayer();

			if (player->CurrentRoom->GetCurrentRoom()->GetRoomNumber() == RoomNumber)
			{
				if (SdlCollisionDetection::IsColliding(&player->Bounds, &Bounds))
				{
					generatedEvents.push_back(EventFactory::Get()->CreateGenericEvent(FetchedPickupEventId, GetSubscriberName()));
					generatedEvents.push_back(make_shared<PlayerCollidedWithPickupEvent>(player, shared_from_this()));
					generatedEvents.push_back( GameObjectEventFactory::MakeRemoveObjectEvent(shared_from_this()));					
				}
			}
		}
		return generatedEvents;
	}

	void Pickup::Draw(SDL_Renderer* renderer)
	{
		sprite->Draw(renderer);
	}

	void Pickup::SetBounds()
	{
		Bounds = CalculateBounds(Position, width, height);
	}

	void Pickup::Update(const unsigned long deltaMs)
	{
		sprite->Position.SetX(Position.GetX());
		sprite->Position.SetY(Position.GetY());

		// Progress sprite frame time
		sprite->Update(deltaMs);
	}
}