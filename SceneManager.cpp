#include "SceneManager.h"
#include <list>
#include "tinyxml2.h"
#include "ResourceManager.h"
#include <memory>
#include <list>
#include "SceneChangedEvent.h"
#include "game_object_factory.h"
#include "AddGameObjectToCurrentSceneEvent.h"
using namespace tinyxml2;

extern shared_ptr<event_manager> event_admin;

scene_manager::scene_manager()
{	
	event_admin->raise_event(std::make_shared<scene_changed_event>(1), this); 
	event_admin->subscribe_to_event(LevelChangedEventType, this);
	event_admin->subscribe_to_event(AddGameObjectToCurrentScene, this);
	
	if(!is_initialized)
		is_initialized = true;
}

scene_manager::scene_manager(scene_manager const&)
{
}

vector<shared_ptr<Event>> scene_manager::process_event(const std::shared_ptr<Event> evt)
{	
	if(LevelChangedEventType == evt->m_eventType) // As a Scene/Level manager I'll load the scene/level's resources when I get a level/scene event
	{
		const auto changed_event = std::dynamic_pointer_cast<scene_changed_event>(evt);
		std::string filename;
		switch(changed_event->m_Level)
		{
			case 1:			
				load_scene_file("scene1.xml");
				break;
			case 2:
				load_scene_file("scene2.xml");
				break;
			case 3:
				load_scene_file("scene3.xml");
				break;
			case 4:
				load_scene_file("scene4.xml");
				break;
			default:
				load_scene_file("scene1.xml");
		}
	}
	
	if(AddGameObjectToCurrentScene == evt->m_eventType)
	{
		auto object_added_event = std::dynamic_pointer_cast<AddGameObjectToCurrentSceneEvent>(evt);
		const auto gameObject = object_added_event->GetGameObject();

		// add to last layer of the scene
		layers.back()->game_objects.push_back(gameObject);	
	}

	return vector<shared_ptr<Event>>();
}

shared_ptr<Layer> scene_manager::add_layer(const std::string& name)
{
	auto layer = find_layer(name);
	if(!layer)
	{
		layer = std::make_shared<Layer>();
		layer->m_Name = name;
		layers.push_back(layer);
	}
	return layer;
}

const shared_ptr<Layer> scene_manager::find_layer(const std::string& name)
{
	for(const auto& layer : layers)
		if(layer->m_Name == name)
			return layer;		
	
	return nullptr;
}

void scene_manager::remove_layer(const std::string& name)
{
	for(const auto& layer : layers)
		if(layer->m_Name == name)
			layers.remove(layer);
}

bool compare_layer_order(const shared_ptr<Layer> rhs, const shared_ptr<Layer> lhs)
{
	return lhs->m_ZOrder < rhs->m_ZOrder;
}

void scene_manager::sort_layers()
{
	layers.sort(compare_layer_order);
}

bool scene_manager::load_scene_file(const std::string& filename)
{
	/*

	A Scene is composed of a) resources at b) various positions c) visibility
	
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
		XMLNode* scene_node = doc.FirstChildElement("scene");
		auto scene_id = scene_node->ToElement()->Attribute("id");
		
		if(scene_node) 
		{
			for(auto layer_node = scene_node->FirstChild(); layer_node; layer_node = layer_node->NextSibling()) 
			{				
				auto layer_element = layer_node->ToElement();
				if(layer_element) 
				{
					auto layer = std::make_shared<Layer>();
					layer->m_ZOrder = layers.size();					

					for(auto layer_attributes = layer_element->FirstAttribute(); layer_attributes; layer_attributes = layer_attributes->Next()) 
					{
						std::string name(layer_attributes->Name());
						std::string value(layer_attributes->Value());						
						
						if(name == "name")
						{
							layer->m_Name = name;
							continue;
						}

						if(name == "posx")
						{
							layer->m_PosX = 0;
							continue;
						}

						if(name == "posy") 
						{
							layer->m_PosY = 0;
							continue;
						}

						if(name == "visible")
							layer->m_Visible = value == "true" ? true : false;
					}
					
					for(auto layer_item = layer_node->FirstChild(); layer_item; layer_item = layer_item->NextSibling()) 
					{
						if(std::string(layer_item->Value()) == "objects") 
						{
							for(auto object_node = layer_item->FirstChild(); object_node; object_node = object_node->NextSibling())
							{	
								auto object = object_node->ToElement();
								if(object == nullptr)
									continue;

								layer->game_objects.push_back(game_object_factory::get_instance().build_game_object(object));																
							}
						}
					}
					layers.push_back(layer);
				}
			}
			
			sort_layers(); // We want to draw from zOrder 0 -> onwards (in order)
			return true;
		}
		
		event_admin->raise_event(make_unique<scene_changed_event>(std::stoi(scene_id)), this);
	}
	return false;
}

string scene_manager::get_subscriber_name()
{
	return "scene_manager";
}

