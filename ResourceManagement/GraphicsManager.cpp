#include "GraphicsManager.h"
#include "ResourceManager.h"
#include "Resource.h"
#include <memory>
using namespace std;


GraphicsManager::GraphicsManager()
{
}


GraphicsManager::~GraphicsManager()
{
}

bool GraphicsManager::Init(unsigned width, unsigned height, char * windowTitle)
{
	return false;
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
	element->QueryIntAttribute("level", &level);
	
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
			actor->VDraw(m_MainWindow);
		}
	}
}
