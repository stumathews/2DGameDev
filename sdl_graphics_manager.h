#pragma once
#include <vector>
#include "graphic_asset.h"
#include "tinyxml2.h"
#include <memory>
#include "event_subscriber.h"
#include "SDL.h"
#include "aliases.h"
#include <functional>


class sdl_graphics_manager final : public event_subscriber
{

public:
	static sdl_graphics_manager& get_instance();

	sdl_graphics_manager();
	~sdl_graphics_manager();
	sdl_graphics_manager(sdl_graphics_manager const&)  = delete;

	// ReSharper disable once CppSpecialFunctionWithoutNoexceptSpecification
	sdl_graphics_manager(sdl_graphics_manager &&other);
	
	// ReSharper disable once CppSpecialFunctionWithoutNoexceptSpecification
	sdl_graphics_manager & operator=(sdl_graphics_manager &&other);  // NOLINT(bugprone-exception-escape)
	sdl_graphics_manager& operator=(sdl_graphics_manager const&)  = delete;
		
	
	SDL_Window* window = nullptr; //The window we'll be rendering to
	SDL_Renderer* window_renderer = nullptr; //The window renderer
	SDL_Surface* window_surface = nullptr; 

	std::shared_ptr<asset> create_asset(tinyxml2::XMLElement * asset_xml_element);
	
	bool initialize(const uint width = 800, uint height = 600, const char* window_title = nullptr);	
	void draw_all_actors();

	void draw_current_scene(bool update_window_surface_after_drawing = true) const;
	uint get_screen_width() const { return screen_width;}
	uint get_screen_height() const { return screen_height;}
	
    string get_subscriber_name() override;
	
private:
	game_objects game_objects;	
	uint screen_width = 0;
	uint screen_height = 0;
	void clear_draw_present(function<void(SDL_Renderer* renderer)> &draw_routine_2) const;
	
	events handle_event(const std::shared_ptr<event> the_event) override;
	
};

inline sdl_graphics_manager& sdl_graphics_manager::get_instance()
{
	static sdl_graphics_manager instance;
	return instance;
}

