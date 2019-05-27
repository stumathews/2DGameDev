#pragma once
#include "tinyxml2.h"
#include "Layer.h"
#include <list>
#include "EventSubscriber.h"

// Scene manager deals with managing the actors that make up a scene.
// Actors are chlidren to layers
class SceneManager
{
public:
	 static SceneManager& getInstance()
        {
            static SceneManager instance;			
            return instance;
        }
		SceneManager(SceneManager const&)  = delete;
        void operator=(SceneManager const&)  = delete;
		std::list<shared_ptr<Layer>> m_Layers;
		
		bool loadSceneFromXml(std::string filename);
		void addObjectsToLayer(shared_ptr<Layer> layer, tinyxml2::XMLElement* element);
		shared_ptr<Layer> addLayer(std::string name);
		shared_ptr<Layer> findLayer(std::string name);
		void removeLayer(std::string name);
		void sortLayers();
		void update();
protected:
	
		
private:	
		
	SceneManager();
	~SceneManager();
};

