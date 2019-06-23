#pragma once
#include "tinyxml2.h"
#include "Layer.h"
#include <list>
#include "EventSubscriber.h"
#include <iostream>

// Scene manager deals with managing the actors that make up a scene.
// Actors are chlidren to layers
class CurrentLevelManager : public EventSubscriber
{
public:
	 static CurrentLevelManager& GetInstance()
        {
            static CurrentLevelManager instance;			
            return instance;
        }
		CurrentLevelManager(CurrentLevelManager const&)  = delete;
        void operator=(CurrentLevelManager const&)  = delete;
		std::list<shared_ptr<Layer>> m_Layers;
		
		bool LoadScene(std::string filename);
		void addObjectsToLayer(shared_ptr<Layer> layer, tinyxml2::XMLElement* element);
		shared_ptr<Layer> addLayer(std::string name);
		shared_ptr<Layer> findLayer(std::string name);
		void removeLayer(std::string name);
		void sortLayers();
		void update();
		void ProcessEvent(std::shared_ptr<Event> evt);
		void Initialize();
		bool m_Initialized;
protected:
	
		
private:	
		
	CurrentLevelManager();
	~CurrentLevelManager(){};
};

