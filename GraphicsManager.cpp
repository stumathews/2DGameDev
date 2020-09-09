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

extern shared_ptr<event_manager> event_admin;
extern shared_ptr<scene_manager> scene_admin;

sdl_graphics_manager::sdl_graphics_manager() : event_subscriber()
{
	event_admin->subscribe_to_event(PlayerMovedEventType, this);
}

vector<shared_ptr<Event>> sdl_graphics_manager::process_event(const std::shared_ptr<Event> the_event)
{
	if(the_event->m_eventType == PlayerMovedEventType)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_FillRect(window_surface, nullptr, 0);
		SDL_RenderClear(renderer);
		draw_current_scene();		
	}
	return vector<shared_ptr<Event>>();
}

string sdl_graphics_manager::get_subscriber_name()
{
	return "sdl_graphics_manager";
}

SDL_Window* get_sdl_window(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const char* title)
{
	const auto out_window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,SDL_WINDOW_SHOWN);	
	if(out_window == nullptr)
	{
		std::cout << "Window could not be created:" << const_cast<char*>(SDL_GetError()) << std::endl;
	}
	return out_window;
}

SDL_Renderer* get_sdl_window_renderer(SDL_Window* window)
{
	const auto window_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if(window_renderer == nullptr)
	{
		std::cout << "Renderer could not be created: " << const_cast<char*>(SDL_GetError()) << std::endl;
	}
	SDL_SetRenderDrawColor(window_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	return window_renderer;
}


bool sdl_graphics_manager::initialize(unsigned int width, unsigned int height, const char * window_title)
{
	logger::log_message("sdl_graphics_manager::Initialize()");
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0)
	{
		logger::log_message(string("SDL could not initialize:") + const_cast<char*>(SDL_GetError()));
		return false;
	}

	if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
	{
		logger::log_message(string("SDL_image could not initialize:") + const_cast<char*>(SDL_GetError()));
		return false;
	}

	window = get_sdl_window(static_cast<int>(width), static_cast<int>(height), window_title);	
	window_surface = SDL_GetWindowSurface(window);
	renderer = get_sdl_window_renderer(window);
	screen_height = height;
	screen_width = width;
	
	return true;
}

std::shared_ptr<asset> sdl_graphics_manager::create_asset(tinyxml2::XMLElement * element)
{	
	int uuid;
	const char* type;
	const char* path;
	const char* name_c;
	int level;
	auto is_animated = false;
	auto num_key_frames = 12, keyFrameHeight = 64, keyFrameWidth = 64;

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
			is_animated = value == "true" ? true : false;
 		}
		if(name == "numKeyFrames")
		{
			num_key_frames = atoi(value.c_str());
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


	auto resource = is_animated
		                        ? std::make_shared<GraphicsResource>(uuid, name_c, path, type, level, num_key_frames, keyFrameHeight, keyFrameWidth,  is_animated)
		                        : std::make_shared<GraphicsResource>(uuid, name_c, path, type, level, is_animated);
		
	
	return resource;
}

void sdl_graphics_manager::DrawAllActors()
{
	for(const auto &actor : game_objects)
		if(actor->is_visible)
			actor->draw(renderer);
	//SDL_RenderPresent(m_Renderer);
	//SDL_UpdateWindowSurface(m_Window);
}

// Draws all the actors in the scene
void sdl_graphics_manager::draw_current_scene(bool updateWindowSurfaceAfterDrawing) const
{
	
	SDL_SetRenderDrawColor(renderer, 0x255, 0x255, 0x55, 0xFF);
	
	for(const auto& layer : scene_admin->layers)
	{
		if(layer->m_Visible)
		{
			for(const auto& game_object : layer->game_objects)
			{
				if(game_object->is_visible) {					
					game_object->draw(renderer);
				}
			}
		}
	}
	
	SDL_UpdateWindowSurface(window);	
}

sdl_graphics_manager::~sdl_graphics_manager()
{
	// get rid of renderer
	SDL_DestroyRenderer(renderer);	
	
	// get rid of window and this will also cleanup the screen surface
	SDL_DestroyWindow(window);
}