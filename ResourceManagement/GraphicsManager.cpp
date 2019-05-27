#include "GraphicsManager.h"
#include "ResourceManager.h"
#include "Resource.h"
#include <memory>
#include "SceneManager.h"
#include "SceneItem.h"
#include "SDL_image.h"
#include <iostream>

using namespace std;


GraphicsManager::GraphicsManager()
{
	// get rid of renderer
	SDL_DestroyRenderer(m_Renderer);	
	
	// get rid of window and this will also cleanup the screen surface
	SDL_DestroyWindow(m_Window);
}


GraphicsManager::~GraphicsManager()
{
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



bool GraphicsManager::Init(unsigned int width, unsigned int height, const char * windowTitle)
{
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

}

std::shared_ptr<Resource> GraphicsManager::MakeResource(tinyxml2::XMLElement * element)
{
	
	int uuid;
	const char* type;
	const char* path;
	const char* name;
	int level;

	element->QueryIntAttribute("uid", &uuid);
	element->QueryStringAttribute("type", &type);
	element->QueryStringAttribute("filename", &path);
	element->QueryStringAttribute("name", &name);
	element->QueryIntAttribute("scene", &level);
	
	// Read anything specific to graphics in the element here...


	auto graphicsResource = shared_ptr<GraphicsResource>(new GraphicsResource(uuid, name, path, type, level));
	
	return graphicsResource;
}

void GraphicsManager::DrawAllActors()
{
	for(auto actor : m_Actors)
	{
		if(actor->m_Visible)
		{
			// Ask the actors to draw themselves please
			actor->VDraw(m_Renderer);
		}
	}
}

// Draws all the actors in the scene
void GraphicsManager::DrawScene()
{
	SDL_SetRenderDrawColor(m_Renderer, 0x255, 0x255, 0x55, 0xFF);
	SDL_RenderClear(m_Renderer);
	for( auto layer : SceneManager::getInstance().m_Layers)
	{
		if(layer->m_Visible){
			for( auto actor : layer->m_Items)
			{
				if(actor->m_Visible) {
					actor->VDraw(m_Renderer);
				}
			}
		}
	}
	SDL_RenderPresent(m_Renderer);
}
