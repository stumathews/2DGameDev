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


std::shared_ptr<asset> resource_manager::get_resource_by_name(const string& name)
{
	return resource_by_name[name];		
}

std::shared_ptr<asset> resource_manager::get_resource_by_uuid(const int uuid)
{
	return resources_by_uuid[uuid];
}

vector<shared_ptr<event>> resource_manager::process_event(const std::shared_ptr<event> evt)
{
	if(evt->type == LevelChangedEventType)
	{
		const auto level = dynamic_pointer_cast<scene_changed_event>(evt)->scene;
		
		// Load all the resources required by the scene and unload out all those that are not in the scene
		for(const auto& level_resources : resources_by_scene)
		{		
			for(const auto& spec : level_resources.second)
			{
				const auto always_load_resource = spec->m_scene == 0;
				if((spec->m_scene == level || always_load_resource) && !spec->m_IsLoaded)
				{				
					spec->load();
					
					logger::log_message(string("scene: " + std::to_string(spec->m_scene) ) + string(spec->m_name) + " asset loaded.");
					
					loaded_resources_count++;
					unloaded_resources_count--;
				} 
				else if(spec->m_IsLoaded && spec->m_scene != level && !always_load_resource )
				{
					spec->unload();
					
					logger::log_message(string("scene: " + std::to_string(spec->m_scene))  + string(spec->m_name) + " asset unloaded.");
					unloaded_resources_count++;
					loaded_resources_count--;
				}
			}		
		}
	}

	return vector<shared_ptr<event>>();
}

void resource_manager::initialize()
{
	logger::log_message("resource_manager::initialize()");

	parse_game_resources();

	logger::log_message("resource_manager ready.");
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
						the_asset = sdl_graphics_manager::create_asset(element);

					if(strcmp(type, "fx") == 0)
						the_asset = AudioManager::GetInstance().create_asset(element);

					if(the_asset)
						store_asset(the_asset);
					else
						logger::log_message(string("No asset manager defined for ") + type);
				}
			}
		}
	}
}

void resource_manager::store_asset(const std::shared_ptr<asset>& the_asset)
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
