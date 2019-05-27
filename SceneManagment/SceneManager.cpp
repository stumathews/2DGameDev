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
using namespace tinyxml2;

SceneManager::SceneManager()
{
}


SceneManager::~SceneManager()
{
}

// Create a new Actor using the details obtained from within the XML file.
void SceneManager::addObjectsToLayer(shared_ptr<Layer> layer, tinyxml2::XMLElement * element)
{
	
	cout << "Loading objects into layer..." << endl;
	// We will initialize the actor to default conords etc and then override them with actual values from the 
	// XML file

	// Note: It might be worthwhile using a Factory to create the Actors passing in just the XML that will
	// serve as the input to the factory - output will be the actor
	shared_ptr<Actor> actor = shared_ptr<Actor>(new Ball( 800 / 2, 600 / 2, 10, 10, 10, 600 / 2  ));
	
	unsigned int r = 0;
	unsigned int g = 0;
	unsigned int b = 0;
	
	// Parse the attributes of the Actor element (again maybe this should be within a separate factory)
	for(const XMLAttribute* elementAttr = element->FirstAttribute(); elementAttr; elementAttr = elementAttr->Next())
	{
		std::string name = elementAttr->Name();
		std::string value = elementAttr->Value();
		
		// A resource is associated with the actor
		if(name == "resourceId")
		{
			const ResourceManager& rm = ResourceManager::getInstance();
			auto resource = ResourceManager::getInstance().GetResourceByUuid(atoi(value.c_str()));
			auto graphicsResource = std::dynamic_pointer_cast<GraphicsResource>(resource);
			actor->SetGraphicsResource(graphicsResource);
		}
		if(name == "posx")
		{
			actor->posX = atoi(value.c_str());
			continue;
		}
		if(name == "visible")
		{
			if(value == "true")
				actor->m_Visible = true;
			else
				actor->m_Visible = false;
		}
		if(name=="posy")
		{
			actor->posY = atoi(value.c_str());
			continue;
		}
		if(name == "colourKey")
		{
			if(value == "true")
				actor->m_ColourKeyEnabled = true;
			else
				actor->m_ColourKeyEnabled = false;

			continue;
		}
		if(name == "r")
		{
			r = atof(value.c_str());
			continue;
		}
		if(name == "g")
		{
			g = atof(value.c_str());
			continue;
		}
		if(name == "b")
		{
			b = atof(value.c_str());
			continue;
		}
		
	}

	cout << "Loading object at x=" << actor->posX  << " y=" << actor->posY << endl;

	if(actor->m_ColourKeyEnabled)
		actor->SetColourKey(r,g,b);		

	// Subscribe this actor the the move event
	cout << "Subscribing actor to PositionChangedEvent..." << endl;
	EventManager::getInstance().SubscribeToEvent(PositionChangeEventType, actor.get());
		
	// Subscribe the actor to the update event
	cout << "Subscribing actor to DoLogicUpdateEventType..." << endl;
	EventManager::getInstance().SubscribeToEvent(DoLogicUpdateEventType, actor.get());
		
	// Add this object to the layer (which later will be rendered by the GraphicsManager)
	layer->m_Items.push_back(actor);

}

shared_ptr<Layer> SceneManager::addLayer(std::string name)
{
	auto layer = findLayer(name);
	if(!layer){
		layer = shared_ptr<Layer>(new Layer());
		layer->m_Name = name;
		m_Layers.push_back(layer);
	}
	return layer;
}

shared_ptr<Layer> SceneManager::findLayer(std::string name)
{
	for( auto layer : m_Layers)
	{
		if(layer->m_Name == name)
		{
			return layer;
		}
	}
	return NULL;
}

void SceneManager::removeLayer(std::string name)
{
	for(auto layer : m_Layers)
	{
		if(layer->m_Name == name)
		{
			m_Layers.remove(layer);
		}
	}
}

bool compareLayerOrder(shared_ptr<Layer> rhs, shared_ptr<Layer> lhs)
{
	return lhs->m_ZOrder < rhs->m_ZOrder;
}

void SceneManager::sortLayers()
{
	m_Layers.sort(compareLayerOrder);
}


// Collects all actors defined in a scene
// and represents them as layers within the SceneManager
// Notifies the resource manager of the new scene and the resource manager will load in/out appropriate scene resources

bool SceneManager::loadSceneFromXml(std::string filename)
{
	cout << "Loaging new scene " << filename << endl;
	XMLDocument doc;

	// Load the resource file into memory
	doc.LoadFile(filename.c_str());
	if(doc.ErrorID() == 0)
	{		
		
		XMLNode* pResourceTree = doc.FirstChildElement("scene");
		XMLElement* el = pResourceTree->ToElement();
		const char* sceneId = el->Attribute("id");	
		cout << "Scene id is " << sceneId << endl;
		
		if(pResourceTree)
		{
			for(tinyxml2::XMLNode* child = pResourceTree->FirstChild(); child; child = child->NextSibling())
			{
				// Load each layer into the Scene manager...
				XMLElement* element = child->ToElement();
				if(element)
				{
					shared_ptr<Layer> layer = shared_ptr<Layer>(new Layer());
					layer->m_ZOrder = m_Layers.size();					

					for( const XMLAttribute* elemAttr = element->FirstAttribute(); elemAttr; elemAttr = elemAttr->Next())
					{
						std::string name = elemAttr->Name();
						std::string value = elemAttr->Value();						

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
						if(name=="visible")
						{
							if(value == "true")
								layer->m_Visible = true;
							else
								layer->m_Visible = false;
							continue;
						}
						
					}
					cout << "Found new layer " << layer->m_Name << endl;
					// Now load the actors into the layer itself
					for( XMLNode* objs = child->FirstChild(); objs; objs = objs->NextSibling())
					{
						if(std::string(objs->Value()) == "objects")
						{
							for( XMLNode* obj = objs->FirstChild(); obj; obj = obj->NextSibling())
							{
								XMLElement* objElement = obj->ToElement();
								// Depending on the object, it can construct itself from the XML
								addObjectsToLayer(layer, objElement);
							}
						}
					}
					m_Layers.push_back(layer);
				}
			}
			// We want to draw from zOrder 0 -> onwards (in order)
			sortLayers();
			return true;

		}

		// Notify the resource manager that the scene has changed at that those scene reseources
		// should be laoded in and any others be loaded out of memory
		EventManager::getInstance().RegisterEvent(shared_ptr<SceneChangedEvent>(new SceneChangedEvent(atoi(sceneId))));
	}
	return false;
}

void SceneManager::update()
{
}
