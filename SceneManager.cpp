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

vector<shared_ptr<Event>> CurrentLevelManager::ProcessEvent(const std::shared_ptr<Event> evt)
{
	// As a Scene/Level manager I'll load the scene/level's resources when I get a level/scene event
	if(evt->m_eventType == LevelChangedEventType)
	{
		std::shared_ptr<scene_changed_event> cpe = std::dynamic_pointer_cast<scene_changed_event>(evt);
		std::string filename;
		if(cpe->m_Level == 1) {
			filename = "scene1.xml";
		}
		if(cpe->m_Level == 2) {
			filename = "scene2.xml";
		}
		if(cpe->m_Level == 3) {
			filename = "scene3.xml";
		}
		if(cpe->m_Level == 4) {
			filename = "scene4.xml";
		}

		LoadScene(filename);
	}
	if(evt->m_eventType == AddGameObjectToCurrentScene)
	{
		auto cpe = std::dynamic_pointer_cast<AddGameObjectToCurrentSceneEvent>(evt);
		auto gameObject = cpe->GetGameObject();
		// add to last layer of the scene
		m_Layers.back()->m_objects.push_back(gameObject);	
	}

	return vector<shared_ptr<Event>>();
}

shared_ptr<Layer> CurrentLevelManager::addLayer(std::string name)
{
	auto layer = findLayer(name);
	if(!layer){
		layer = shared_ptr<Layer>(new Layer());
		layer->m_Name = name;
		m_Layers.push_back(layer);
	}
	return layer;
}

shared_ptr<Layer> CurrentLevelManager::findLayer(std::string name)
{
	for( auto layer : m_Layers)
		if(layer->m_Name == name)
			return layer;		
	
	return NULL;
}

void CurrentLevelManager::removeLayer(std::string name)
{
	for(auto layer : m_Layers)
		if(layer->m_Name == name)
			m_Layers.remove(layer);
}

bool compareLayerOrder(shared_ptr<Layer> rhs, shared_ptr<Layer> lhs)
{
	return lhs->m_ZOrder < rhs->m_ZOrder;
}

void CurrentLevelManager::sortLayers()
{
	m_Layers.sort(compareLayerOrder);
}

// Reads the scene data from the scene file
bool CurrentLevelManager::LoadScene(std::string filename)
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
		XMLNode* pResourcesNode = doc.FirstChildElement("scene");
		XMLElement* el = pResourcesNode->ToElement();
		const char* sceneId = el->Attribute("id");	 // Every scence has an id
		
		if(pResourcesNode) 
		{
			// Traverse the multiple layers within the scene
			for(tinyxml2::XMLNode* layerNode = pResourcesNode->FirstChild(); layerNode; layerNode = layerNode->NextSibling()) 
			{				
				auto layerElement = layerNode->ToElement();
				if(layerElement) 
				{
					// We'll build up the layer object with the discovered details
					auto layer = shared_ptr<Layer>(new Layer());

					layer->m_ZOrder = m_Layers.size();					
					
					// Traverse layer's details/attributes
					for(const XMLAttribute* attributes = layerElement->FirstAttribute(); attributes; attributes = attributes->Next()) 
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
						
					} // Done with traversing layer
					
					/* Now travese the constituents of the layer, ie the game objects (which reference resources) */
					for(auto data = layerNode->FirstChild(); data; data = data->NextSibling()) 
					{
						// Currently we only support objects within the layer
						if(std::string(data->Value())._Equal("objects")) 
						{
							for(auto objectNode = data->FirstChild(); objectNode; objectNode = objectNode->NextSibling())
							{						
								// Ok what type of object is that - construct it using the game object factory!
								auto object = objectNode->ToElement();
								if(object == NULL)
									continue;
								shared_ptr<GameObject>  gameObject = GameObjectFactory::GetInstance().BuildGameObject(object);
								
								layer->m_objects.push_back(gameObject);	

								event_manager::get_instance().subscribe_to_event(PositionChangeEventType, layer->m_objects.back().get());	
								event_manager::get_instance().subscribe_to_event(DoLogicUpdateEventType, layer->m_objects.back().get());	
								
							}
						}
					}
					m_Layers.push_back(layer);
				}
			}
			
			sortLayers(); // We want to draw from zOrder 0 -> onwards (in order)
			return true;

		}

		// Notify that a new scene has been loaded. 
		// One noticible subscriber ill be the resource manager to load the scene's resources in memory
		event_manager::get_instance().register_event(shared_ptr<scene_changed_event>(new scene_changed_event(atoi(sceneId))));
	}
	return false;
}

void CurrentLevelManager::update()
{
}
