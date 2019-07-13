#include "SceneManager.h"
#include "SceneManager.h"
#include <list>
#include "tinyxml2.h"
#include "Actor.h"
#include "Ball.h"
#include "ResourceManager.h"
#include <memory>
#include <list>
#include "SceneManager.h"
#include "SceneChangedEvent.h"
#include "GameObjectFactory.h"
using namespace tinyxml2;

void CurrentLevelManager::Initialize()
{
	EventManager::GetInstance().SubscribeToEvent(LevelChangedEventType, this);
	m_Initialized = true;
}

CurrentLevelManager::CurrentLevelManager()
{
	if(!m_Initialized)
		m_Initialized = true;
};

void CurrentLevelManager::ProcessEvent(std::shared_ptr<Event> evt)
{
	// As a Scene/Level manager I'll load the scene/level's resources when I get a level/scene event
	if(evt->m_eventType == LevelChangedEventType)
	{
		std::shared_ptr<SceneChangedEvent> cpe = std::dynamic_pointer_cast<SceneChangedEvent>(evt);
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
		PopulateLayers(filename);
	}
}

void CurrentLevelManager::addObjectsToLayer(shared_ptr<Layer> layer, tinyxml2::XMLElement * sceneObjectXml)
{	
	layer->m_objects.push_back(GameObjectFactory::GetInstance().BuildGameObject(sceneObjectXml));	

	EventManager::GetInstance().SubscribeToEvent(PositionChangeEventType, layer->m_objects.back().get());	
	EventManager::GetInstance().SubscribeToEvent(DoLogicUpdateEventType, layer->m_objects.back().get());	
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

bool CurrentLevelManager::PopulateLayers(std::string filename)
{
	XMLDocument doc;
		
	doc.LoadFile(filename.c_str());
	if(doc.ErrorID() == 0) 	{		
		
		XMLNode* pResourceTree = doc.FirstChildElement("scene");
		XMLElement* el = pResourceTree->ToElement();
		const char* sceneId = el->Attribute("id");	
		
		if(pResourceTree) {
			for(tinyxml2::XMLNode* layerNode = pResourceTree->FirstChild(); layerNode; layerNode = layerNode->NextSibling()) {
				// Load each layer into the Scene manager...
				auto layerElement = layerNode->ToElement();
				if(layerElement) {
					auto layer = shared_ptr<Layer>(new Layer());
					layer->m_ZOrder = m_Layers.size();					

					for(const XMLAttribute* layerAttribute = layerElement->FirstAttribute(); layerAttribute; layerAttribute = layerAttribute->Next()) {
						std::string name = layerAttribute->Name();
						std::string value = layerAttribute->Value();						

						if(name == "name"){
							layer->m_Name = name;
							continue;
						}
						if(name =="posx"){
							layer->m_PosX = 0;
							continue;
						}
						if(name=="posy") {
							layer->m_PosY = 0;
							continue;
						}
						if(name=="visible") {
							if(value == "true")
								layer->m_Visible = true;
							else
								layer->m_Visible = false;
							continue;
						}
						
					}
					
					for(auto layerContents = layerNode->FirstChild(); layerContents; layerContents = layerContents->NextSibling()) {
						if(std::string(layerContents->Value()) == "objects") {
							for(auto sceneObject = layerContents->FirstChild(); sceneObject; sceneObject = sceneObject->NextSibling()) {						
								addObjectsToLayer(layer, sceneObject->ToElement());
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
		EventManager::GetInstance().RegisterEvent(shared_ptr<SceneChangedEvent>(new SceneChangedEvent(atoi(sceneId))));
	}
	return false;
}

void CurrentLevelManager::update()
{
}
