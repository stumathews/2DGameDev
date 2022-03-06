#pragma once

#include "scene/Room.h"
#include "objects/DrawingBase.h"
#include <events/IEventSubscriber.h>
#include <objects/MultipleInheritableEnableSharedFromThis.h>

namespace gamelib
{
	// Represents a Pickup in the game
	class Pickup final : public gamelib::DrawingBase
	{
	public:
		
		// Create a new pickup at specific coordinates
		Pickup(int x, int y, int w, int h, bool visible, gamelib::EventManager& event_admin, gamelib::SettingsManager& settings_admin);
		
		// Create a new pickup with undefined coordinates
		Pickup(bool visible, gamelib::SettingsManager& settings_admin, gamelib::EventManager& event_admin);

		// Initialize the pickup
		void init();
		
		/* IEventSubscriber overrides */
		std::string get_subscriber_name() override;
		gamelib::object_type get_type() override;
		std::string get_identifier() override;
		std::vector<std::shared_ptr<gamelib::event>> handle_event(std::shared_ptr<gamelib::event> the_event) override;

		// Load pickup settings
		void load_settings(gamelib::SettingsManager& settings_admin) override;

		// Draw Pickup
		void draw(SDL_Renderer* renderer) override;

		// Update Pickup
		void update() override;
		SDL_Color fill_color;
		gamelib::EventManager& event_admin;
	private:
		int width, height;
	};
}
