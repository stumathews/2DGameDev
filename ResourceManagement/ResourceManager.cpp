#include "ResourceManager.h"
#include "tinyxml2.h"
#include <iostream>
#include <map>
#include "Resource.h"
#include "GraphicsManager.h"
#include <memory>
#include "AudioManager.h"
#include "LevelChangedEvent.h"

using namespace tinyxml2;
using namespace std;

std::shared_ptr<Resource> ResourceManager::GetResourceByName(string name)
{
	auto resource = m_ResourcesByName[name];	
	return resource;
}

std::shared_ptr<Resource> ResourceManager::GetResourceByUuid(int uuid)
{
	auto resource = m_ResourcesByUuid[uuid];
	return resource;
}

void ResourceManager::SetCurrentLevel(int newLevel)
{
	cout << "Setting current level to " << newLevel << endl;
	// Load all the resources required by the level
	// and unload all those that don't
	for(auto levelResources : m_ResourcesByLevel)
	{
		auto level = levelResources.first;
		auto resources = levelResources.second;
		
		for( auto resource : resources )
		{
			if((resource->m_level == newLevel || resource->m_level == 0) && !resource->m_IsLoaded){
				resource->VLoad();
				m_CountLoadedResources++;
				m_CountUnloadedResources--;
			} 
			// Don't unload level 0 resources - they are always needed irrespective of the level
			else if(resource->m_IsLoaded && resource->m_level != 0 && resource->m_level != newLevel)
			{
				resource->VUnload();
				m_CountUnloadedResources++;
				m_CountLoadedResources--;
			}
		}		
	}
}

void ResourceManager::ProcessEvent(std::shared_ptr<Event> evt)
{
	switch(evt->m_eventType)
	{
		case LevelChangedEventType:
			auto cpe = std::dynamic_pointer_cast<LevelChangedEvent>(evt);
			SetCurrentLevel(cpe->m_Level);
			break;
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
						resource = GraphicsManager::getInstance().MakeResource(element);						
					}
					if(strcmp(type, "fx") == 0)
					{					
						resource = AudioManager::getInstance().MakeResource(element);						
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

	// artificially load of first level

	SetCurrentLevel(1);
}