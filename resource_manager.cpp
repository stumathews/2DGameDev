#include "resource_manager.h"
#include "tinyxml2.h"
#include <iostream>
#include <map>
#include "asset.h"
#include "sdl_graphics_manager.h"
#include <memory>
#include "AudioManager.h"
#include "SceneChangedEvent.h"
#include "Common.h"
#include "font_manager.h"

using namespace tinyxml2;
using namespace std;

extern shared_ptr<global_config> config;
extern shared_ptr<event_manager> event_admin;

resource_manager::resource_manager() = default;

vector<shared_ptr<event>> resource_manager::handle_event(const shared_ptr<event> evt)
{
	if(evt->type == event_type::LevelChangedEventType)
	{
		log_message("Detected level change. Loading level assets...");
		load_level_assets(dynamic_pointer_cast<scene_changed_event>(evt)->scene_id);
		log_message("Level assets loaded.");
	}

	return vector<shared_ptr<event>>();
}

/**
 Load all the resources required by the scene and unload out all those that are not in the scene.
 */
void resource_manager::load_level_assets(const int level)
{	
	for(const auto& level_resources : resources_by_scene) // we need access to all resources to swap in/out resources
	{
		for(const auto& asset : level_resources.second)
		{
			const auto always_load_resource = asset->scene == 0;
			if((asset->scene == level || always_load_resource) && !asset->is_loaded)
			{				
				asset->load();
					
				logger::log_message(string("scene: " + to_string(asset->scene) ) + string(asset->name) + " asset loaded.");
					
				loaded_resources_count++;
				unloaded_resources_count--;
			} 
			else if(asset->is_loaded && asset->scene != level && !always_load_resource )
			{
				asset->unload();
					
				logger::log_message(string("scene: " + to_string(asset->scene))  + string(asset->name) + " asset unloaded.");
				unloaded_resources_count++;
				loaded_resources_count--;
			}
		}		
	}
}

void resource_manager::unload()
{
	log_message("Unloading all resources...");
	for(auto iterator = begin(resource_by_name); iterator != end(resource_by_name); ++iterator)
	{
		auto &asset_name = iterator->first;
		auto &asset = iterator->second;
		
		asset->unload();
		
		log_message("Unloaded asset '" + asset_name + string("'."));
	}	
}

bool resource_manager::initialize()
{
	return run_and_log("resource_manager::initialize()", config->verbose, [&]()
	{	
		// we will load the resources for the level that has been loaded
		event_admin->subscribe_to_event(event_type::LevelChangedEventType, this);
		return true;
	});
}

/**
 Index Resources.xml file
 */
void resource_manager::read_resources()
{	
	logger::log_message("resource_manager: reading resources.xml.");
	
	XMLDocument doc;
	
	doc.LoadFile( "resources.xml" ); // Load the list of resources
	
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

					if(strcmp(type, "fx") == 0 || strcmp(type, "music") == 0)
						the_asset = audio_manager::get_instance().create_asset(element);

					if(strcmp(type, "font") == 0)
						the_asset = font_manager::get_instance().create_asset(element);

					if(the_asset)
						store_asset(the_asset);
					else
						logger::log_message(string("No asset manager defined for ") + type);
				}
			}
		}
	}

	log_message(to_string(resource_count) + string(" assets available in resource manager."));
}

void resource_manager::store_asset(const shared_ptr<asset>& the_asset)
{
	// assets are explicitly associated with a scene that it will work in
	resources_by_scene[the_asset->scene].push_back(the_asset);

	// Index asset by its name
	resource_by_name.insert(pair<string, shared_ptr<asset>>(the_asset->name, the_asset));

	// Index the asset by its id
	resources_by_uuid.insert(pair<int, shared_ptr<asset>>(the_asset->uid, the_asset));

	log_message("Discovered " + string(the_asset->type) + string(" asset#: ") + to_string(the_asset->uid) + string(" ") + string(the_asset->name));
	resource_count++;
}

string resource_manager::get_subscriber_name()
{
	return "resource manager";	
}

shared_ptr<asset> resource_manager::get(const string& name)
{
	return resource_by_name[name];		
}

shared_ptr<asset> resource_manager::get(const int uuid)
{
	return resources_by_uuid[uuid];
}
