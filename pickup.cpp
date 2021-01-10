#include "pickup.h"

pickup::pickup(const int number, const int x, const int y, const int rw, const int rh,
               const std::shared_ptr<gamelib::resource_manager>& resource_admin, const bool fill,
               const bool supports_move_logic,
               const bool is_visible, const std::shared_ptr<gamelib::settings_manager>& settings_admin)
: square(number, x, y, rw, rh, resource_admin, fill, supports_move_logic, is_visible, settings_admin, gamelib::square_role::Pickup)
{
}

std::vector<std::shared_ptr<gamelib::event>> pickup::handle_event(std::shared_ptr<gamelib::event> event)
{
	return square::handle_event(event);
}

void pickup::load_settings(std::shared_ptr<gamelib::settings_manager> settings_admin)
{
	return square::load_settings(settings_admin);
}

void pickup::draw(SDL_Renderer* renderer)
{
	return square::draw(renderer);
}

void pickup::update()
{
}
