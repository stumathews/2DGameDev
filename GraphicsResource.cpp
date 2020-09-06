#include "GraphicsResource.h"
#include <iostream>
#include <SDL_image.h>
#include "GraphicsManager.h"
#include <SDL.h>




//GraphicsResource::GraphicsResource(int uid, string name, string path, string type, int level, unsigned int numKeyFrames, unsigned int keyFrameHeight, unsigned int keyFrameWidth)
//	: GraphicsResource(uid, name, path, type, level)
//{	
//	m_NumKeyFrames = numKeyFrames;
//	m_KeyFrameHeight = keyFrameHeight;
//	m_KeyFrameWidth = keyFrameWidth;
//	m_bIsAnimated = m_NumKeyFrames > 0;
//
//}

GraphicsResource::~GraphicsResource()
{
	if(m_IsLoaded){
		std::cout << "Destroying graphics resource: " << m_name << std::endl;
	}
	GraphicsResource::unload();
}

void GraphicsResource::load()
{
	unload(); // unload if its already loaded

	const auto tmp_surface = IMG_Load(m_path.c_str());
	if(tmp_surface)
	{
		auto r = sdl_graphics_manager::get().m_WindowSurface;
				
		m_Surface =  SDL_ConvertSurface(tmp_surface, r->format, NULL);
		newTexture = SDL_CreateTextureFromSurface( sdl_graphics_manager::get().m_Renderer, m_Surface );
		
		SDL_FreeSurface(tmp_surface);
		if(m_Surface)
			m_IsLoaded = true;
	}
}

void GraphicsResource::unload()
{

	if(m_Surface)
	{
		SDL_FreeSurface(m_Surface);
		m_Surface = nullptr;
	}
	m_IsLoaded = false;
}
