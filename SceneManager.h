#pragma once
#include "tinyxml2.h"
#include "Layer.h"
#include <list>
#include "EventSubscriber.h"
#include <iostream>

// Scene manager deals with managing the actors that make up a scene.
// Actors are chlidren to layers
class CurrentLevelManager : public IEventSubscriber
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
		
		/* Collects all actors defined in a scene and represents them as layers within the SceneManager
		*  Notifies the resource manager of the new scene and the resource manager will load in/out appropriate scene resources
		*/
		bool load_scene(const std::string& filename);
		
		shared_ptr<Layer> addLayer(std::string name);
		const shared_ptr<Layer> findLayer(const std::string name);
		void removeLayer(std::string name);
		void sort_layers();
		static void update() {}
		vector<shared_ptr<Event>> process_event(std::shared_ptr<Event> evt) override;
		/*
		* Initialize the Current level manager
		* Subscribe to event system
		*/
		void Initialize();
		bool m_Initialized;
protected:
	
		
private:	
		
	CurrentLevelManager();
	~CurrentLevelManager(){};
};

