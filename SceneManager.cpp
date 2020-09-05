#include "SceneManager.h"
#include "SceneManager.h"
#include <list>
#include "tinyxml2.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include <memory>
#include <list>
#include "SceneManager.h"
#include "SceneChangedEvent.h"
#include "GameObjectFactory.h"
#include "AddGameObjectToCurrentSceneEvent.h"
#include "constants.h"
using namespace tinyxml2;

void CurrentLevelManager::Initialize()
{
	event_manager::get_instance().subscribe_to_event(LevelChangedEventType, this);
	event_manager::get_instance().subscribe_to_event(AddGameObjectToCurrentScene, this);
	
	m_Initialized = true;
}

CurrentLevelManager::CurrentLevelManager()
{
	if(!m_Initialized)
		m_Initialized = true;
};

vector<shared_ptr<Event>> CurrentLevelManager::process_event(const std::shared_ptr<Event> evt)
{	
	if(LevelChangedEventType == evt->m_eventType) // As a Scene/Level manager I'll load the scene/level's resources when I get a level/scene event
	{
		const auto changed_event = std::dynamic_pointer_cast<scene_changed_event>(evt);
		std::string filename;
		switch(changed_event->m_Level)
		{
			case 1:			
				load_scene("scene1.xml");
				break;
			case 2:
				load_scene("scene2.xml");
				break;
			case 3:
				load_scene("scene3.xml");
				break;
			case 4:
				load_scene("scene4.xml");
				break;
			default:
				load_scene("scene1.xml");
		}
	}
	
	if(AddGameObjectToCurrentScene == evt->m_eventType)
	{
		auto object_added_event = std::dynamic_pointer_cast<AddGameObjectToCurrentSceneEvent>(evt);
		const auto gameObject = object_added_event->GetGameObject();

		// add to last layer of the scene
		m_Layers.back()->m_objects.push_back(gameObject);	
	}

	return vector<shared_ptr<Event>>();
}

shared_ptr<Layer> CurrentLevelManager::addLayer(const std::string name)
{
	auto layer = findLayer(name);
	if(!layer)
	{
		layer = std::make_shared<Layer>();
		layer->m_Name = name;
		m_Layers.push_back(layer);
	}
	return layer;
}

const shared_ptr<Layer> CurrentLevelManager::findLayer(const std::string name)
{
	for(const auto& layer : m_Layers)
		if(layer->m_Name == name)
			return layer;		
	
	return nullptr;
}

void CurrentLevelManager::removeLayer(std::string name)
{
	for(const auto& layer : m_Layers)
		if(layer->m_Name == name)
			m_Layers.remove(layer);
}

bool compare_layer_order(shared_ptr<Layer> rhs, shared_ptr<Layer> lhs)
{
	return lhs->m_ZOrder < rhs->m_ZOrder;
}

void CurrentLevelManager::sort_layers()
{
	m_Layers.sort(compare_layer_order);
}

// Reads the scene data from the scene file
bool CurrentLevelManager::load_scene(const std::string& filename)
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
			// Traverse the multiple layers within the scene
			for(auto layer_node = scene_node->FirstChild(); layer_node; layer_node = layer_node->NextSibling()) 
			{				
				auto layer_element = layer_node->ToElement();
				if(layer_element) 
				{
					// We'll build up the layer object with the discovered details
					auto layer = std::make_shared<Layer>();

					layer->m_ZOrder = m_Layers.size();					
					
					// Traverse layer's details/attributes
					for(auto attributes = layer_element->FirstAttribute(); attributes; attributes = attributes->Next()) 
					{
						std::string name = attributes->Name();
						std::string value = attributes->Value();						
						
						// Populate the layer with the layer's details as we come across them
						if(name._Equal("name"))
						{
							layer->m_Name = name;
							continue;
						}

						if(name._Equal("posx"))
						{
							layer->m_PosX = 0;
							continue;
						}

						if(name._Equal("posy")) 
						{
							layer->m_PosY = 0;
							continue;
						}

						if(name._Equal("visible")) 
						{
							if(value == "true")
								layer->m_Visible = true;
							else
								layer->m_Visible = false;
							continue;
						}
						
					} // Done with traversing layer attributes
					
					/* Now traverse the constituents of the layer, ie the game objects (which reference resources) */
					for(auto layer_item = layer_node->FirstChild(); layer_item; layer_item = layer_item->NextSibling()) 
					{
						if(std::string(layer_item->Value())._Equal("objects")) 
						{
							for(auto object_node = layer_item->FirstChild(); object_node; object_node = object_node->NextSibling())
							{	
								auto object = object_node->ToElement();
								if(object == nullptr)
									continue;

								auto game_object = GameObjectFactory::GetInstance().BuildGameObject(object);
								layer->m_objects.push_back(game_object);	

								// Subscribe all game objects to some basic event types
								event_manager::get_instance().subscribe_to_event(PositionChangeEventType, game_object.get());	
								event_manager::get_instance().subscribe_to_event(DoLogicUpdateEventType, game_object.get());								
							}
						}
					}
					m_Layers.push_back(layer);
				}
			}
			
			sort_layers(); // We want to draw from zOrder 0 -> onwards (in order)
			return true;

		}
		
		event_manager::get_instance().raise_event(make_unique<scene_changed_event>(atoi(scene_id)));
	}
	return false;
}

