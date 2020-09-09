#include "resource_manager.h"
#include "tinyxml2.h"
#include <iostream>
#include <map>
#include "asset.h"
#include "sdl_graphics_manager.h"
#include <memory>
#include "AudioManager.h"
#include "SceneChangedEvent.h"
#include "Logger.h"

using namespace tinyxml2;
using namespace std;

extern shared_ptr<event_manager> event_admin;

resource_manager::resource_manager()
{
	event_admin->subscribe_to_event(LevelChangedEventType, this);
}


std::shared_ptr<asset> resource_manager::get_resource_by_name(string name)
{
	auto resource = resource_by_name[name];	
	return resource;
}

std::shared_ptr<asset> resource_manager::get_resource_by_uuid(int uuid)
{
	auto resource = resources_by_uuid[uuid];
	return resource;
}

void resource_manager::load_current_scene_resources(const int new_level)
{
	// Load all the resources required by the scene and unload all those that don't
	for(const auto& level_resources : resources_by_scene)
	{		
		for(const auto& spec : level_resources.second)
		{
			if((spec->m_scene == new_level || spec->m_scene == 0) && !spec->m_IsLoaded)
			{				
				spec->load();
				logger::log_message(string("scene: ") + std::to_string(spec->m_scene)  + " asset loaded." + spec->m_name);
				
				loaded_resources_count++;
				unloaded_resources_count--;
			} 
			// Don't unload level 0 resources - they are always needed irrespective of the level
			else if(spec->m_IsLoaded && spec->m_scene != 0 && spec->m_scene != new_level)
			{
				spec->unload();
				logger::log_message(string("scene: ") + std::to_string(spec->m_scene)  + " asset unloaded." + spec->m_name);
				unloaded_resources_count++;
				loaded_resources_count--;
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
	
	

	resource_count = 0;
	loaded_resources_count = 0;
	unloaded_resources_count = 0;

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
					shared_ptr<asset> the_asset = nullptr;
					const char* type;

					element->QueryStringAttribute("type", &type);			

					if(strcmp(type, "graphic") == 0)
					{
						the_asset = sdl_graphics_manager::create_asset(element);						
					}
					
					if(strcmp(type, "fx") == 0)
					{					
						the_asset = AudioManager::GetInstance().create_asset(element);						
					}
						
					if(the_asset)
					{
						store_asset(the_asset);
						continue;
					}
				}
			}
		}
	}
}

void resource_manager::store_asset(std::shared_ptr<asset> the_asset)
{
	resources_by_scene[the_asset->m_scene].push_back(the_asset);
	resource_by_name.insert(std::pair<string, std::shared_ptr<asset>>(the_asset->m_name, the_asset));
	resources_by_uuid.insert(std::pair<int, std::shared_ptr<asset>>(the_asset->m_uid, the_asset));
	resource_count++;
}

string resource_manager::get_subscriber_name()
{
	return "resource manager";	
}
