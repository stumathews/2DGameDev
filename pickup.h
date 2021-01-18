#pragma once

#include "scene/Room.h"
#include "objects/DrawingBase.h"
#include <events/IEventSubscriber.h>
#include <objects/MultipleInheritableEnableSharedFromThis.h>

namespace gamelib
{
	class Pickup final : public gamelib::DrawingBase
	{
	public:
		Pickup(int x, int y, int w, int h, bool visible, const std::shared_ptr<gamelib::event_manager> event_admin, const std::shared_ptr<gamelib::settings_manager>& settings_admin);
		void init();
		Pickup(bool visible, const std::shared_ptr<gamelib::settings_manager>& settings_admin);

		std::string get_subscriber_name() override { return "pickup";};

		gamelib::object_type get_type() override { return gamelib::object_type::pickup; }

		std::string get_identifier() override { return "pickup"; }

		std::vector<std::shared_ptr<gamelib::event>>
		handle_event(std::shared_ptr<gamelib::event> the_event) override;

		void load_settings(std::shared_ptr<gamelib::settings_manager> settings_admin) override;
		void draw(SDL_Renderer* renderer) override;
		void update() override;
		SDL_Color fill_color;
		std::shared_ptr<gamelib::event_manager> event_admin;
		virtual ~Pickup();
	private:
		int width, height;
	};
}
