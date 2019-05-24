#include "GraphicsResourceManager.h"
#include "ResourceManager.h"
#include "Resource.h"
#include <memory>
using namespace std;


GraphicsResourceManager::GraphicsResourceManager()
{
}


GraphicsResourceManager::~GraphicsResourceManager()
{
}

std::shared_ptr<Resource> GraphicsResourceManager::MakeResource(tinyxml2::XMLElement * element)
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
	
	// collect a list of grap
	m_GraphicsResources.push_back(graphicsResource);
	return graphicsResource;
}
