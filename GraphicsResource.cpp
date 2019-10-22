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
	VUnload();
}

void GraphicsResource::VLoad()
{
	VUnload(); // unload if its already loaded
	

	SDL_Surface* tmpSurface = IMG_Load(m_path.c_str());
	if(tmpSurface)
	{
		auto r = SDLGraphicsManager::GetInstance().m_WindowSurface;
		
		
		m_Surface =  SDL_ConvertSurface(tmpSurface, r->format, NULL);
		newTexture = SDL_CreateTextureFromSurface( SDLGraphicsManager::GetInstance().m_Renderer, m_Surface );
		
		SDL_FreeSurface(tmpSurface);
		if(m_Surface)
		{
			m_IsLoaded = true;
		}		
	}
}

void GraphicsResource::VUnload()
{

	if(m_Surface)
	{
		SDL_FreeSurface(m_Surface);
		m_Surface = NULL;
	}
	m_IsLoaded = false;
}
