#include "ResourceManager.h"
#include "tinyxml2.h"
#include <iostream>
#include <map>
#include "asset.h"
#include "GraphicsManager.h"
#include <memory>
#include "AudioManager.h"
#include "SceneChangedEvent.h"
#include "Logger.h"

using namespace tinyxml2;
using namespace std;


resource_manager::resource_manager()
{
}

std::shared_ptr<asset> resource_manager::get_resource_by_name(string name)
{
	auto resource = m_ResourcesByName[name];	
	return resource;
}

std::shared_ptr<asset> resource_manager::get_resource_by_uuid(int uuid)
{
	auto resource = m_ResourcesByUuid[uuid];
	return resource;
}

void resource_manager::load_current_scene_resources(const int new_level)
{
	// Load all the resources required by the scene and unload all those that don't
	for(const auto& level_resources : m_ResourcesByLevel)
	{		
		for(const auto& spec : level_resources.second)
		{
			if((spec->m_scene == new_level || spec->m_scene == 0) && !spec->m_IsLoaded)
			{				
				spec->load();
				logger::log_message(string("scene: ") + std::to_string(spec->m_scene)  + " asset loaded." + spec->m_name);
				
				loaded_resources_count_++;
				unloaded_resources_count_--;
			} 
			// Don't unload level 0 resources - they are always needed irrespective of the level
			else if(spec->m_IsLoaded && spec->m_scene != 0 && spec->m_scene != new_level)
			{
				spec->unload();
				logger::log_message(string("scene: ") + std::to_string(spec->m_scene)  + " asset unloaded." + spec->m_name);
				unloaded_resources_count_++;
				loaded_resources_count_--;
			}
		}		
	}
}

vector<shared_ptr<Event>> resource_manager::process_event(const std::shared_ptr<Event> evt)
{
	switch(evt->m_eventType)
	{
		case LevelChangedEventType:
			// As a resource manager I'll actually Load the resources for the scene into memory
			auto cpe = std::dynamic_pointer_cast<scene_changed_event>(evt);
			load_current_scene_resources(cpe->m_Level);
			break;
	}
	return vector<shared_ptr<Event>>();
}

void resource_manager::initialize()
{
	logger::log_message("resource_manager::initialize()");
	
	event_manager::get().subscribe_to_event(LevelChangedEventType, this);

	resource_count_ = 0;
	loaded_resources_count_ = 0;
	unloaded_resources_count_ = 0;

	parse_game_resources();
}


void resource_manager::parse_game_resources()
{	
	logger::log_message("resource_manager: reading resources.xml.");
	
	XMLDocument doc;

	// Load the list of resources 
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
					shared_ptr<asset> resource = nullptr;
					const char* type;

					element->QueryStringAttribute("type", &type);			

					if(strcmp(type, "graphic") == 0)
					{
						resource = sdl_graphics_manager::make_resource_spec(element);						
					}
					if(strcmp(type, "fx") == 0)
					{					
						resource = AudioManager::GetInstance().make_resource_spec(element);						
					}
						
					if(resource)
					{
						store_resource(resource);
						continue;
					}
				}
			}
		}
	}
}

void resource_manager::store_resource(std::shared_ptr<asset> resource)
{
	m_ResourcesByLevel[resource->m_scene].push_back(resource);
	m_ResourcesByName.insert(std::pair<string, std::shared_ptr<asset>>(resource->m_name, resource));
	m_ResourcesByUuid.insert(std::pair<int, std::shared_ptr<asset>>(resource->m_uid, resource));
	resource_count_++;
}

string resource_manager::get_subscriber_name()
{
	return "resource manager";
	
}
