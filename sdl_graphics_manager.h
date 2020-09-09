#pragma once
#include <vector>
#include "GraphicsResource.h"
#include "tinyxml2.h"
#include <memory>
#include "event_subscriber.h"
#include "SDL.h"

class game_object;

// Manages the graphics in the project,
// specifically graphic-enabled objects like Actors and AnimatedActors.

class sdl_graphics_manager final : public event_subscriber
{	
	std::vector<shared_ptr<game_object>> game_objects;	
	unsigned int screen_width = 0;
	unsigned int screen_height = 0;
	vector<shared_ptr<Event>> process_event(const std::shared_ptr<Event> the_event) override;
public:
	static sdl_graphics_manager& get();
	sdl_graphics_manager(sdl_graphics_manager const&)  = delete;
	bool initialize(unsigned int width = 800, unsigned int height = 600, const char* window_title = nullptr);
	    
    void operator=(sdl_graphics_manager const&)  = delete;
	
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Surface* window_surface = nullptr;
	
			
	static std::shared_ptr<asset> create_asset(tinyxml2::XMLElement * assetXmlElement);
	void DrawAllActors();
	void draw_current_scene(bool updateWindowSurfaceAfterDrawing = true) const;
	unsigned int get_screen_width() const { return  screen_width;}
	unsigned int get_screen_height() const { return  screen_height;}
	~sdl_graphics_manager();
	sdl_graphics_manager();
    string get_subscriber_name() override;
};

inline sdl_graphics_manager& sdl_graphics_manager::get()
{
	static sdl_graphics_manager instance;
	return instance;
}

