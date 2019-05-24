#include "GraphicsResource.h"
#include <iostream>


GraphicsResource::GraphicsResource(int uid, string name, string path, string type, int level) 
	: Resource(uid, name, path, type, level)
{	
}

GraphicsResource::~GraphicsResource()
{
}

void GraphicsResource::VLoad()
{
	std::cout << "Loading graphics resource into memory: " << m_name << std::endl;
}

void GraphicsResource::VUnload()
{
}
