#pragma once
#include <vector>
#include "GraphicsResource.h"
#include "tinyxml2.h"
#include <memory>
#include "EventSubscriber.h"
#include "SDL.h"

class game_object;

// Manages the graphics in the project,
// specifically graphic-enabled objects like Actors and AnimatedActors.

class sdl_graphics_manager : public event_subscriber
{
public:
	 static sdl_graphics_manager& get()
        {
            static sdl_graphics_manager instance;			
            return instance;
        }
		sdl_graphics_manager(sdl_graphics_manager const&)  = delete;
        void operator=(sdl_graphics_manager const&)  = delete;
		
		SDL_Window* m_Window = nullptr;
		SDL_Renderer* m_Renderer = nullptr;
		SDL_Surface* m_WindowSurface = nullptr;
		
		/* Initializes the graphics system
		* Creates Main Window
		* Sets up the window surface and renderer for drawing	
		*/
		bool Initialize(unsigned int width = 800, unsigned int height=600, const char* windowTitle=0);
		// Creates a graphics Resource
	 static std::shared_ptr<asset> make_resource_spec(tinyxml2::XMLElement * assetXmlElement);
		
		// Draw all the Actors we know about onto the surface
		void DrawAllActors();

		void draw_current_scene(bool updateWindowSurfaceAfterDrawing = true) const;
		unsigned int GetScreenWidth() { return  m_ScreenWidth;}
		unsigned int GetScreenhEIGHT() { return  m_ScreenHeight;}
private:	

	std::vector<shared_ptr<game_object>> m_Actors;
	sdl_graphics_manager()
		: m_Window(NULL), 
		m_Renderer(NULL),
		m_WindowSurface(NULL),
		m_ScreenHeight(0),
		m_ScreenWidth(0) { }
	~sdl_graphics_manager();
	unsigned int m_ScreenWidth;
	unsigned int m_ScreenHeight;


	// Inherited via IEventSubscriber
	vector<shared_ptr<Event>> process_event(const std::shared_ptr<Event> evt) override;

 public:
	 string get_subscriber_name() override;
};

