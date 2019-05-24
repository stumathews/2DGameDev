#include "Actor.h"
#include <iostream>

//
//
//Actor::~Actor()
//{
//}

Actor::~Actor()
{
	std::cout << "Deleting actor!" << std::endl;
}

void Actor::SetGraphicsResource(GraphicsResource * graphicsResource)
{
	m_GraphicsResource = graphicsResource;
}
