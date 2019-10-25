#include "GraphicsManager.h"
#include "ResourceManager.h"
#include "Resource.h"
#include <memory>
#include "SceneManager.h"
#include "SceneItem.h"
#include "SDL_image.h"
#include "GraphicsResource.h"
#include <iostream>
#include <SDL.h>
using namespace std;

SDLGraphicsManager::~SDLGraphicsManager()
{
	
	// get rid of renderer
	SDL_DestroyRenderer(m_Renderer);	
	
	// get rid of window and this will also cleanup the screen surface
	SDL_DestroyWindow(m_Window);
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



bool SDLGraphicsManager::Initialize(unsigned int width, unsigned int height, const char * windowTitle)
{
	std::cout << "Initalizing Graphics" << std::endl;
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

	
	return true;
}

std::shared_ptr<Resource> SDLGraphicsManager::MakeResource(tinyxml2::XMLElement * element)
{
	
	int uuid;
	const char* type;
	const char* path;
	const char* name;
	int level;
	bool isAnimated = false;
	int numKeyFrames = 12, keyFrameHeight = 64, keyFrameWidth = 64;

	element->QueryIntAttribute("uid", &uuid);
	element->QueryStringAttribute("type", &type);
	element->QueryStringAttribute("filename", &path);
	element->QueryStringAttribute("name", &name);
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
	
	
	shared_ptr<GraphicsResource> graphicsResource;
	graphicsResource= isAnimated 
		? 	shared_ptr<GraphicsResource>(new GraphicsResource(uuid, name, path, type, level, numKeyFrames, keyFrameHeight, keyFrameWidth, isAnimated))
		:   shared_ptr<GraphicsResource>(new GraphicsResource(uuid, name, path, type, level, isAnimated));
		
	
	return graphicsResource;
}

void SDLGraphicsManager::DrawAllActors()
{
	for(auto actor : m_Actors)
	{
		if(actor->m_Visible)
		{
			// Ask the actors to draw themselves please
			actor->VDraw(m_Renderer);
		}
	}
	//SDL_RenderPresent(m_Renderer);
	//SDL_UpdateWindowSurface(m_Window);
}

// Draws all the actors in the scene
void SDLGraphicsManager::DrawCurrentScene(bool updateWindowSurfaceAfterDrawing)
{
	
	SDL_SetRenderDrawColor(m_Renderer, 0x255, 0x255, 0x55, 0xFF);
	SDL_FillRect(m_WindowSurface, 0, 0);
	
	static bool sendSurfaceToScreen = true;

	// Draw objects in layers, which are ordered by z-order
	for(auto layer : CurrentLevelManager::GetInstance().m_Layers)
	{
		if(layer->m_Visible)
		{
			for(auto actor : layer->m_objects)
			{
				if(actor->m_Visible) {					
					actor->VDraw(m_Renderer);
				}
			}
		}
	}
	
	if(updateWindowSurfaceAfterDrawing)
		SDL_UpdateWindowSurface(m_Window);
	else
		SDL_RenderPresent(m_Renderer);
	
}
