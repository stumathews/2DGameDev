#include "GraphicsManager.h"
#include "ResourceManager.h"
#include "asset.h"
#include <memory>
#include "SceneManager.h"
#include "SceneItem.h"
#include "SDL_image.h"
#include "GraphicsResource.h"
#include <iostream>
#include <SDL.h>
#include "EventSubscriber.h"
#include "Logger.h"
using namespace std;

sdl_graphics_manager::~sdl_graphics_manager()
{
	
	// get rid of renderer
	SDL_DestroyRenderer(m_Renderer);	
	
	// get rid of window and this will also cleanup the screen surface
	SDL_DestroyWindow(m_Window);
}


vector<shared_ptr<Event>> sdl_graphics_manager::process_event(const std::shared_ptr<Event> evt)
{
	if(evt->m_eventType == PlayerMovedEventType)
	{
		SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0);
		SDL_FillRect(m_WindowSurface, 0, 0);
		SDL_RenderClear(m_Renderer);
		draw_current_scene();
		
	}
	return vector<shared_ptr<Event>>();
}

string sdl_graphics_manager::get_subscriber_name()
{
	return "sdl_graphics_manager";
}

SDL_Window* GetSDLWindow(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const char* title)
{
	SDL_Window* outWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
				SDL_WINDOW_SHOWN);	
	if(outWindow == NULL)
	{
		std::cout << "Window could not be created:" << (char*)SDL_GetError() << std::endl;
	}
	return outWindow;
}

SDL_Renderer* GetSDLWindowRenderer(SDL_Window* window)
{
	SDL_Renderer* outRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if(outRenderer == NULL)
	{
		std::cout << "Renderer could not be created: " << (char*)SDL_GetError() << std::endl;
	}
	SDL_SetRenderDrawColor(outRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	return outRenderer;
}



bool sdl_graphics_manager::Initialize(unsigned int width, unsigned int height, const char * windowTitle)
{
	logger::log_message("sdl_graphics_manager::Initialize()");
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0)
	{
		std::cout << "SDL could not initialize!" << (char*)SDL_GetError() << std::endl;
		return false;
	}

	// Initialize SDL Image extension	
	if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
	{
		std::cout << "SDL_image could not initialize!" << (char*)SDL_GetError() << std::endl;
		return false;
	}

	m_Window = GetSDLWindow(width, height, windowTitle);
	
	m_WindowSurface = SDL_GetWindowSurface(m_Window);
	m_Renderer = GetSDLWindowRenderer(m_Window);

	m_ScreenHeight = height;
	m_ScreenWidth = width;

	event_manager::get().subscribe_to_event(PlayerMovedEventType, this);
	
	return true;
}

std::shared_ptr<asset> sdl_graphics_manager::make_resource_spec(tinyxml2::XMLElement * element)
{
	
	int uuid;
	const char* type;
	const char* path;
	const char* name_c;
	int level;
	bool isAnimated = false;
	int numKeyFrames = 12, keyFrameHeight = 64, keyFrameWidth = 64;

	element->QueryIntAttribute("uid", &uuid);
	element->QueryStringAttribute("type", &type);
	element->QueryStringAttribute("filename", &path);
	element->QueryStringAttribute("name", &name_c);
	element->QueryIntAttribute("scene", &level);
	
	for(const XMLAttribute* elementAttr = element->FirstAttribute(); elementAttr; elementAttr = elementAttr->Next())
	{
		std::string name = elementAttr->Name();
		std::string value = elementAttr->Value();

		if(name == "isAnimated")
		{
			isAnimated = value == "true" ? true : false;
 		}
		if(name == "numKeyFrames")
		{
			numKeyFrames = atoi(value.c_str());
		}
		if(name == "keyFrameHeight")
		{
			keyFrameHeight = atoi(value.c_str());
		}
		if(name == "keyFrameWidth")
		{
			keyFrameWidth = atoi(value.c_str());
		}
	}


	auto resource = isAnimated
		                        ? std::make_shared<GraphicsResource>(uuid, name_c, path, type, level, numKeyFrames, keyFrameHeight, keyFrameWidth,  isAnimated)
		                        : std::make_shared<GraphicsResource>(uuid, name_c, path, type, level, isAnimated);
		
	
	return resource;
}

void sdl_graphics_manager::DrawAllActors()
{
	for(auto actor : m_Actors)
	{
		if(actor->is_visible)
		{
			// Ask the actors to draw themselves please
			actor->draw(m_Renderer);
		}
	}
	//SDL_RenderPresent(m_Renderer);
	//SDL_UpdateWindowSurface(m_Window);
}

// Draws all the actors in the scene
void sdl_graphics_manager::draw_current_scene(bool updateWindowSurfaceAfterDrawing) const
{
	
	SDL_SetRenderDrawColor(m_Renderer, 0x255, 0x255, 0x55, 0xFF);
	
	for(const auto& layer : scene_manager::get().m_Layers)
	{
		if(layer->m_Visible)
		{
			for(const auto& game_object : layer->m_objects)
			{
				if(game_object->is_visible) {					
					game_object->draw(m_Renderer);
				}
			}
		}
	}
	
	SDL_UpdateWindowSurface(m_Window);	
}
