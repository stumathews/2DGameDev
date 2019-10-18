#include "GameObjectBase.h"
#include <iostream>
#include "GraphicsManager.h"


void GameObjectBase::VDraw(SDL_Renderer* renderer)
{
	// By default a game object can draw its resource onto the rendering surface
	SDL_Rect drawLocation = { m_xPos, m_yPos };	
	auto rect = GetResource()->m_bIsAnimated ?  &m_GraphicsResource->m_viewPort : NULL;
	SDL_BlitSurface(m_GraphicsResource->m_Surface, rect, SDLGraphicsManager::GetInstance().m_WindowSurface, &drawLocation);	
}

GameObjectBase::~GameObjectBase()
{
	std::cout << "~Actor()" << std::endl;
}

