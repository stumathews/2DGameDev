#include "GameObjectBase.h"
#include <iostream>
#include "GraphicsManager.h"


void GameObjectBase::DrawResource(SDL_Renderer* renderer)
{
	if(GetResource() != NULL)
	{
	   // By default a game object can draw its resource onto the rendering surface
		SDL_Rect drawLocation = { m_xPos, m_yPos, 100,100 };	
		auto rect = GetResource()->m_bIsAnimated ?  &m_GraphicsResource->m_viewPort : NULL;
		SDL_BlitSurface(m_GraphicsResource->m_Surface, rect, SDLGraphicsManager::GetInstance().m_WindowSurface, &drawLocation);	
		//SDL_RenderCopy(renderer, GetResource()->newTexture, NULL, &drawLocation);
	}
}

GameObjectBase::~GameObjectBase()
{
	std::cout << "~Actor()" << std::endl;
}

