#pragma once

#include "Room.h"
#include "objects/DrawableGameObject.h"
#include <objects/AnimatedSprite.h>
#include <objects/MultipleInheritableEnableSharedFromThis.h>
#include <util/Tuple.h>

namespace gamelib
{
	/// <summary>
	/// Represents a Pickup in the game
	/// </summary>
	class Pickup final : public DrawableGameObject, public inheritable_enable_shared_from_this<Pickup>
	{
	public:
		
		/// <summary>
		/// Create a new pickup at specific coordinates
		/// </summary>
		Pickup(const std::string name, const std::string type, const int x, const int y, const int width, const int height, bool visible, const int inRoomNumber) 
			: DrawableGameObject(name, type, gamelib::Coordinate<int>(x, y), IsVisible)
		{
			this->width = width;
			this->height = height;
			this->RoomNumber = inRoomNumber;
		}
		
		/// <summary>
		/// Create a new pickup with undefined coordinates
		/// </summary>
		/// <param name="visible">initial visible state</param>
		Pickup(const bool visible) : DrawableGameObject(0, 0, visible)
		{
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
		std::string GetSubscriberName() override { return "pickup"; }

		/// <summary>
		/// Provide Pickup game type
		/// </summary>
		/// <returns>Pickup</returns>
		GameObjectType GetGameObjectType() override { return GameObjectType::Pickup; }

		/// <summary>
		/// Provide name
		/// </summary>
		/// <returns>Name</returns>
		std::string GetName() override { return "pickup"; }

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
		void LoadSettings() override { }

		/// <summary>
		/// Draw Pickup
		/// </summary>
		/// <param name="renderer">SDL renderer</param>
		void Draw(SDL_Renderer* renderer) override;

		/// <summary>
		///  Update Pickup
		/// </summary>
		void Update(float deltaMs) override;

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
		std::shared_ptr<AnimatedSprite> _sprite;
	};
}
