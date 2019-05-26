#include "GraphicsResource.h"
#include <iostream>
#include <SDL_image.h>
#include "GraphicsManager.h"
#include <SDL.h>

GraphicsResource::GraphicsResource(int uid, string name, string path, string type, int level) 
	: Resource(uid, name, path, type, level)
{	
}

GraphicsResource::~GraphicsResource()
{
}

void GraphicsResource::VLoad()
{
	VUnload(); // unload if its already loaded
	std::cout << "Loading graphics resource into memory: " << m_name << std::endl;
	/*SDL_Surface* tmpSurface = IMG_Load(m_path.c_str());
	if(tmpSurface)
	{
		auto r = GraphicsManager::getInstance().m_MainWindow;
		auto format = r->
		m_Surface =  SDL_ConvertSurface(tmpSurface, format, NULL);
		
		SDL_FreeSurface(tmpSurface);
		if(m_Surface)
		{
			m_IsLoaded = true;
		}		
	}	*/
}

void GraphicsResource::VUnload()
{
	std::cout << "Unloading graphics resource out of memory: " << m_name << std::endl;
	/*if(m_Surface)
	{
		SDL_FreeSurface(m_Surface);
		m_Surface = NULL;
	}*/
	m_IsLoaded = false;
}
