#include "ResourceManager.h"
#include "tinyxml2.h"
#include <iostream>
#include <map>
#include "Resource.h"
#include "GraphicsResourceManager.h"
#include <memory>
#include "AudioResourceManager.h"

using namespace tinyxml2;
using namespace std;

std::shared_ptr<Resource> ResourceManager::GetResourceByName(string name)
{
	auto resource = m_ResourcesByName[name];	
	return resource;
}

void ResourceManager::SetCurrentLevel(int level)
{
	// call the load() function on all the resources that are in specified for the current level
	auto levelResources = m_ResourcesByLevel[level];
	for(auto resource : levelResources)
	{
		resource->VLoad(); // Loads the resource in memory
	}
}

void ResourceManager::ReadInResources()
{	
	std::cout << "Loading all the resources from the Resources.xml ..." << std::endl;;
	
	XMLDocument doc;

	// Load the resource file into memory
	doc.LoadFile( "resources.xml" );
	if(doc.ErrorID() == 0)
	{		
		XMLNode* pResourceTree = doc.FirstChildElement("Assets");
			
		if(pResourceTree)
		{
			for(tinyxml2::XMLNode* child = pResourceTree->FirstChild(); child; child = child->NextSibling())
			{
				XMLElement* element = child->ToElement();
				if(element)
				{
					shared_ptr<Resource> resource = NULL;
					const char* type;

					element->QueryStringAttribute("type", &type);			

					if(strcmp(type, "graphic") == 0)
					{
						resource = GraphicsResourceManager::getInstance().MakeResource(element);						
					}
					if(strcmp(type, "fx") == 0)
					{					
						resource = AudioResourceManager::getInstance().MakeResource(element);						
					}						
						
					if(resource)
					{
						StoreResource(resource);
						continue;
					}
				}
			}
		}
	}

	// artificiall load of first level

	SetCurrentLevel(1);
}