#pragma once
#include <vector>
#include "graphic_resource.h"
#include "../../tinyxml2.h"
#include <memory>
#include "SDL.h"
#include <functional>

#include "events/event_subscriber.h"


class sdl_graphics_manager final : public event_subscriber
{

public:
	
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

	void draw_current_scene(bool update_window_surface_after_drawing = true) const;
	uint get_screen_width() const { return screen_width;}
	uint get_screen_height() const { return screen_height;}
	
    std::string get_subscriber_name() override;

	static std::shared_ptr<graphic_resource> to_resource(const std::shared_ptr<asset>& asset);
	
private:
	uint screen_width = 0;
	uint screen_height = 0;
	void clear_draw_present(std::function<void(SDL_Renderer* renderer)> &render_routine) const;
	
	events handle_event(const std::shared_ptr<event> the_event) override;
	
};
