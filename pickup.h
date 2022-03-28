#pragma once

#include "scene/Room.h"
#include "objects/DrawableGameObject.h"
#include <events/IEventSubscriber.h>

namespace gamelib
{
	/// <summary>
	/// Represents a Pickup in the game
	/// </summary>
	class Pickup final : public gamelib::DrawableGameObject
	{
	public:
		
		/// <summary>
		/// Create a new pickup at specific coordinates
		/// </summary>
		Pickup(int x, int y, int width, int height, bool visible);
		
		/// <summary>
		/// Create a new pickup with undefined coordinates
		/// </summary>
		/// <param name="visible">initial visible state</param>
		/// <param name="settingsManager">settingsManager</param>
		/// <param name="eventManager">eventManager</param>
		Pickup(bool visible);

		/// <summary>
		/// Initialize the pickup
		/// </summary>
		void Initialize();
		
		/// <summary>
		/// Provide name to event system
		/// </summary>
		/// <returns>Subscriber name</returns>
		std::string GetSubscriberName() override;

		/// <summary>
		/// Provide Pickup game type
		/// </summary>
		/// <returns>Pickup</returns>
		gamelib::object_type GetGameObjectType() override;

		/// <summary>
		/// Provide name
		/// </summary>
		/// <returns>Name</returns>
		std::string GetName() override;

		/// <summary>
		/// Handle pickup events
		/// </summary>
		/// <param name="event">incoming event</param>
		/// <returns>generated events</returns>
		std::vector<std::shared_ptr<gamelib::Event>> HandleEvent(std::shared_ptr<gamelib::Event> event) override;

		/// <summary>
		/// Load pickup settings
		/// </summary>
		/// <param name="settingsManager">settingsManager</param>
		void LoadSettings() override;

		/// <summary>
		/// Draw Pickup
		/// </summary>
		/// <param name="renderer">SDL renderer</param>
		void Draw(SDL_Renderer* renderer) override;

		/// <summary>
		///  Update Pickup
		/// </summary>
		void Update() override;
		
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
		/// Colour of the pickup
		/// </summary>
		SDL_Color fillColour;
	};
}
