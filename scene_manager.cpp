#include "scene_manager.h"
#include <list>
#include "tinyxml2.h"
#include "resource_manager.h"
#include <memory>
#include <list>
#include "SceneChangedEvent.h"
#include "game_object_factory.h"
#include "AddGameObjectToCurrentSceneEvent.h"
#include <algorithm>

using namespace tinyxml2;
extern shared_ptr<event_manager> event_admin;

scene_manager::scene_manager()
{
	// Emit event to switch to scene 1 on initial construction of scene manager
	event_admin->raise_event(std::make_shared<scene_changed_event>(1), this);

	// I care about when the level changes
	event_admin->subscribe_to_event(LevelChangedEventType, this);

	// I care about when I'm asked to add game object to current scene
	event_admin->subscribe_to_event(AddGameObjectToCurrentScene, this);
	
	if(!is_initialized)
		is_initialized = true;
}

vector<shared_ptr<event>> scene_manager::process_event(const std::shared_ptr<event> evt)
{
	// load in new scene
	if(LevelChangedEventType == evt->type) 
		load_new_scene(evt);	

	// add new object to scene (last layer)
	if(AddGameObjectToCurrentScene == evt->type)
		add_to_scene(std::dynamic_pointer_cast<AddGameObjectToCurrentSceneEvent>(evt)->GetGameObject());	
	
	return vector<shared_ptr<event>>();
}


void scene_manager::load_new_scene(const std::shared_ptr<event> evt)
{
	const auto scene =  std::dynamic_pointer_cast<scene_changed_event>(evt)->scene;

	auto raise_scene_loaded_event = [this](int scene_id){ /* event_admin->raise_event(make_unique<scene_changed_event>(scene_id), this);*/ };
	
	switch(scene)
	{
	case 1:			
		load_scene_file("scene1.xml");
		raise_scene_loaded_event(scene);
		break;
	case 2:
		load_scene_file("scene2.xml");
		raise_scene_loaded_event(scene);
		break;
	case 3:
		load_scene_file("scene3.xml");
		raise_scene_loaded_event(scene);
		break;
	case 4:
		load_scene_file("scene4.xml");
		raise_scene_loaded_event(scene);
		break;
	default:
		load_scene_file("scene1.xml");
		raise_scene_loaded_event(scene);
	}
}

void scene_manager::add_to_scene(const std::shared_ptr<game_object> game_object)
{
	// add to last layer of the scene
	layers.back()->game_objects.push_back(game_object);
}

shared_ptr<layer> scene_manager::add_layer(const std::string& name)
{
	auto the_layer = find_layer(name);
	if(!the_layer)
	{
		the_layer = std::make_shared<layer>();
		the_layer->name = name;
		layers.push_back(the_layer);
	}
	return the_layer;
}

shared_ptr<layer> scene_manager::find_layer(const std::string& name)
{
	for(const auto& layer : layers)
		if(layer->name == name)
			return layer;		
	
	return nullptr;
}

void scene_manager::remove_layer(const std::string& name)
{
	for(const auto& layer : layers)
		if(layer->name == name)
			layers.remove(layer);
}

bool compare_layer_order(const shared_ptr<layer> rhs, const shared_ptr<layer> lhs)
{
	return lhs->zorder < rhs->zorder;
}

void scene_manager::sort_layers()
{
	//std::sort(begin(layers), end(layers)); // can we use this?
	layers.sort(compare_layer_order);
}

std::list<shared_ptr<layer>> scene_manager::get_layers() const
{
	return layers;
}

bool scene_manager::load_scene_file(const std::string& filename)
{	
	/* A Scene is composed of a) resources at b) various positions c) visibility
	
	<scene id="2">
	  <layer name="layer0" posx="0" posy="0" visible="true">
		<objects>
		  <object posx="100" posy="40" resourceId="7" visible="true" colourKey="true" r="0" g="0" b="0"></object>
		  <object posx="500" posy="40" resourceId="8" visible="true" colourKey="true" r="0" g="0" b="0"></object>
		</objects>
	  </layer>
	</scene>
	
	Read in the scene details and store them in the scene manager
	*/

	XMLDocument doc;		
	doc.LoadFile(filename.c_str());
	if(doc.ErrorID() == 0) 	
	{				
		XMLNode* scene = doc.FirstChildElement("scene");
		auto scene_id = scene->ToElement()->Attribute("id");
		
		if(scene) // <scene id="2">
		{
			for(auto layer_node = scene->FirstChild(); layer_node; layer_node = layer_node->NextSibling()) //  <layer ...>
			{				
				auto layer_element = layer_node->ToElement();
				if(layer_element) 
				{
					// build up a layer object from scene file
					auto the_layer = std::make_shared<layer>();
					the_layer->zorder = layers.size();					

					for(auto layer_attributes = layer_element->FirstAttribute(); layer_attributes; layer_attributes = layer_attributes->Next()) // // <layer name="layer0" posx="0" posy="0" visible="true"
					{						
						// populate the new layer object:
						
						const string key(layer_attributes->Name());
						const string value(layer_attributes->Value());						

						if(key == "name")
						{
							the_layer->name = key;
							continue;
						}

						if(key == "posx")
						{
							the_layer->x = 0;
							continue;
						}

						if(key == "posy") 
						{
							the_layer->y = 0;
							continue;
						}

						if(key == "visible")
							the_layer->visible = value == "true" ? true : false;
					}

					// Process contents of the layer 
					for(auto layer_item = layer_node->FirstChild(); layer_item; layer_item = layer_item->NextSibling()) // <object ...
					{
						if(std::string(layer_item->Value()) == "objects") 
						{
							for(auto object_node = layer_item->FirstChild(); object_node; object_node = object_node->NextSibling())
							{	
								auto object = object_node->ToElement();
								if(object == nullptr)
									continue;

								// populate the list of game objects in this layer
								the_layer->game_objects.push_back(game_object_factory::get_instance().build_game_object(object));																
							}
						}
					}

					// populate the number of layers in this scene
					layers.push_back(the_layer);
				}
			}
			
			sort_layers(); // We want to draw from zOrder 0 -> onwards (in order)
			return true;
		} // finished processing scene, layers populated

	}
	return false;
}

string scene_manager::get_subscriber_name()
{
	return "scene_manager";
}

