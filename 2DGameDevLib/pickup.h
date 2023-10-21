#pragma once

#include "Room.h"
#include "objects/DrawableGameObject.h"
#include <objects/AnimatedSprite.h>
#include <util/Tuple.h>

#include "SpriteAsset.h"

namespace gamelib
{
	/// <summary>
	/// Represents a Pickup in the game
	/// </summary>
	class Pickup final : public DrawableGameObject, public std::enable_shared_from_this<Pickup>
	{
	public:
		/// <summary>
		/// Create a new pickup at specific coordinates
		/// </summary>
		Pickup(const std::string name, const std::string type, const int x, const int y, const int width,
		       const int height, const bool visible, const int inRoomNumber)
			: DrawableGameObject(name, type, gamelib::Coordinate<int>(x, y), visible)
		{
			this->IsVisible = visible;
			this->width = width;
			this->height = height;
			this->RoomNumber = inRoomNumber;
		}

		Pickup(const std::string name, const std::string type, const Coordinate<int> startingPoint, const bool visible,
		       const int inRoomNumber, const std::shared_ptr<SpriteAsset> asset)
			: DrawableGameObject(name, type, gamelib::Coordinate<int>(startingPoint.GetX(), startingPoint.GetY()),
			                     visible)
		{
			this->IsVisible = visible;
			this->Asset = asset;
			this->width = asset->Dimensions.GetWidth();
			this->height = asset->Dimensions.GetHeight();
			this->RoomNumber = inRoomNumber;
		}

		/// <summary>
		/// Create a new pickup with undefined coordinates
		/// </summary>
		/// <param name="visible">initial visible state</param>
		Pickup(const bool visible) : DrawableGameObject(0, 0, visible)
		{
			this->IsVisible = visible;
			this->width = 0;
			this->height = 0;
			this->RoomNumber = 0;
		}

		/// <summary>
		/// The Pickup's room number
		/// </summary>
		int RoomNumber;

		/// <summary>
		/// Initialize the pickup
		/// </summary>
		void Initialize();

		/// <summary>
		/// Provide name to event system
		/// </summary>
		/// <returns>Subscriber name</returns>
		std::string GetSubscriberName() override { return Name; }

		/// <summary>
		/// Provide Pickup game type
		/// </summary>
		/// <returns>Pickup</returns>
		GameObjectType GetGameObjectType() override { return GameObjectType::Pickup; }

		/// <summary>
		/// Provide name
		/// </summary>
		/// <returns>Name</returns>
		std::string GetName() override { return Name; }

		/// <summary>
		/// Handle pickup events
		/// </summary>
		/// <param name="event">incoming event</param>
		/// <param name="deltaMs">Delta time in milliseconds</param>
		/// <returns>generated events</returns>
		std::vector<std::shared_ptr<Event>> HandleEvent(std::shared_ptr<Event> event, unsigned long deltaMs) override;

		/// <summary>
		/// Load pickup settings
		/// </summary>
		void LoadSettings() override
		{
		}

		/// <summary>
		/// Draw Pickup
		/// </summary>
		/// <param name="renderer">SDL renderer</param>
		void Draw(SDL_Renderer* renderer) override;

		/// <summary>
		///  Update Pickup
		/// </summary>
		void Update(unsigned long deltaMs) override;

		/**
		 * \brief The asset associated with the sprite
		 */
		std::shared_ptr<Asset> Asset;

	protected:
		/// <summary>
		/// Update our bounds
		/// </summary>
		void SetBounds();

	private:
		/// <summary>
		/// Pickup width
		/// </summary>
		int width;

		/// <summary>
		/// Pickup height
		/// </summary>
		int height;

		/// <summary>
		/// Animated sprite
		/// </summary>
		std::shared_ptr<AnimatedSprite> sprite;
	};
}
