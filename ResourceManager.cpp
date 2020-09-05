#include "ResourceManager.h"
#include "tinyxml2.h"
#include <iostream>
#include <map>
#include "Resource.h"
#include "GraphicsManager.h"
#include <memory>
#include "AudioManager.h"
#include "SceneChangedEvent.h"

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

void ResourceManager::LoadCurrentSceneResources(int newLevel)
{
	// Load all the resources required by the scene
	// and unload all those that don't
	for(auto levelResources : m_ResourcesByLevel)
	{
		auto level = levelResources.first;
		auto resources = levelResources.second;
		
		for(const auto& resource : resources)
		{
			const auto is_already_loaded = resource->m_IsLoaded;
			if((resource->m_scene == newLevel || resource->m_scene == 0) && !is_already_loaded){
				std::cout << "(" << resource->m_scene << ")" << "MEM_LOAD:" << resource->m_name << std::endl;
				resource->VLoad();
				m_CountLoadedResources++;
				m_CountUnloadedResources--;
			} 
			// Don't unload level 0 resources - they are always needed irrespective of the level
			else if(resource->m_IsLoaded && resource->m_scene != 0 && resource->m_scene != newLevel)
			{
				std::cout << "(" << resource->m_scene << ")" << "MEM_UNLOAD:" << resource->m_name << std::endl;
				resource->VUnload();
				m_CountUnloadedResources++;
				m_CountLoadedResources--;
			}
		}		
	}
}

vector<shared_ptr<Event>> ResourceManager::process_event(const std::shared_ptr<Event> evt)
{
	switch(evt->m_eventType)
	{
		case LevelChangedEventType:
			// As a resource manager I'll actually Load the resources for the scene into memory
			auto cpe = std::dynamic_pointer_cast<scene_changed_event>(evt);
			LoadCurrentSceneResources(cpe->m_Level);
			break;
	}
	return vector<shared_ptr<Event>>();
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
			for(auto child = pResourceTree->FirstChild(); child; child = child->NextSibling())
			{
				const auto element = child->ToElement();
				if(element)
				{
					shared_ptr<Resource> resource = nullptr;
					const char* type;

					element->QueryStringAttribute("type", &type);			

					if(strcmp(type, "graphic") == 0)
					{
						resource = SDLGraphicsManager::make_resource(element);						
					}
					if(strcmp(type, "fx") == 0)
					{					
						resource = AudioManager::GetInstance().make_resource(element);						
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
}