#pragma once

#include "Room.h"
#include "objects/DrawableGameObject.h"
#include <character/AnimatedSprite.h>
#include <geometry/Coordinate.h>

#include "asset/SpriteAsset.h"

namespace gamelib
{
	class Pickup final : public DrawableGameObject, public std::enable_shared_from_this<Pickup>
	{
	public:
		Pickup(const std::string name, const std::string type, const int x, const int y, const int width,
		       const int height, const bool visible, const int inRoomNumber)
			: DrawableGameObject(name, type, Coordinate(x, y), visible)
		{
			this->IsVisible = visible;
			this->width = width;
			this->height = height;
			this->RoomNumber = inRoomNumber;
		}

		Pickup(const std::string name, const std::string type, const Coordinate<int> startingPoint, const bool visible,
		       const int inRoomNumber, const std::shared_ptr<SpriteAsset> asset)
			: DrawableGameObject(name, type, Coordinate(startingPoint.GetX(), startingPoint.GetY()),
			                     visible)
		{
			this->IsVisible = visible;
			this->Asset = asset;
			this->width = asset->Dimensions.GetWidth();
			this->height = asset->Dimensions.GetHeight();
			this->RoomNumber = inRoomNumber;
		}

		explicit Pickup(const bool visible) : DrawableGameObject(0, 0, visible)
		{
			this->IsVisible = visible;
			this->width = 0;
			this->height = 0;
			this->RoomNumber = 0;
		}

		GameObjectType GetGameObjectType() override { return GameObjectType::Pickup; }

		std::string GetSubscriberName() override { return Name; }
		std::string GetName() override { return Name; }
		std::vector<std::shared_ptr<Event>> HandleEvent(std::shared_ptr<Event> event, unsigned long deltaMs) override;

		void Initialize();
		void Draw(SDL_Renderer* renderer) override;
		void Update(unsigned long deltaMs) override;

		int RoomNumber;
		std::shared_ptr<Asset> Asset;

	protected:
		void SetBounds();

	private:
		int width;
		int height;
		std::shared_ptr<AnimatedSprite> sprite;
	};
}
