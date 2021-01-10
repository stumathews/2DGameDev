#pragma once
#include "scene/Square.h"

class pickup final : public gamelib::square
{
public:
	pickup(const int number, const int x, const int y, const int rw, const int rh,
	       const std::shared_ptr<gamelib::resource_manager>& resource_admin, const bool fill,
	       const bool supports_move_logic, const bool is_visible,
	       const std::shared_ptr<gamelib::settings_manager>& settings_admin);

	std::string get_subscriber_name() override { return "pickup";};
	gamelib::object_type get_type() override { return gamelib::object_type::pickup; }
	std::string get_identifier() override { return "pickup"; }
	std::vector<std::shared_ptr<gamelib::event>> handle_event(std::shared_ptr<gamelib::event> event) override;
	void load_settings(std::shared_ptr<gamelib::settings_manager> settings_admin) override;
	void draw(SDL_Renderer* renderer) override;
	void update() override;
};

