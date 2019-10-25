#pragma once
#include <string>
#include <map>
#include "Resource.h"
#include "tinyxml2.h"
#include <memory>
#include "tinyxml2.h"
#include "GraphicsManager.h"
#include "EventSubscriber.h"
#include "EventManager.h"
#include "Events.h"
#include <iostream>
using namespace std;
using namespace tinyxml2;


class ResourceManager : IEventSubscriber
{
    public:
        static ResourceManager& GetInstance()
        {
            static ResourceManager instance;			
            return instance;
        }
		ResourceManager(ResourceManager const&)  = delete;
        void operator=(ResourceManager const&)  = delete;

		// Get Resource By Name
		std::shared_ptr<Resource> GetResourceByName(string name);

		// Get a resource by its uuid
		std::shared_ptr<Resource> GetResourceByUuid(int uuid);

		// Get the number of resources available
		int GetResourceCount(){ return m_ResourceCount; }
		// Load all the resources in the current level and unload those not in the current level
		void LoadCurrentSceneResources(int level);

		// Process an incoming event
		void ProcessEvent(std::shared_ptr<Event> evt);
		
		/*
		* Initialize the resource manager. 
		* Subscribe to event manager events
		* Read in all game resource references(meta data)
		*/
		void Initialize()
		{
			// Subscribe to scene changed Event
			EventManager::GetInstance().SubscribeToEvent(LevelChangedEventType, this);
			
			m_ResourceCount, m_CountLoadedResources, m_CountUnloadedResources = 0;
			// Load the resources on creation of the Resource Manager
			ReadInResources();
			// Load all the global game rsources - default is set to level 0
		}
    private:
		
		ResourceManager(){ }
		void ReadInResources();
		
				
		void StoreResource(std::shared_ptr<Resource> resource)
		{	
			m_ResourcesByLevel[resource->m_scene].push_back(resource);
			m_ResourcesByName.insert(std::pair<string, std::shared_ptr<Resource>>(resource->m_name, resource));	
			m_ResourcesByUuid.insert(std::pair<int, std::shared_ptr<Resource>>(resource->m_uid, resource));
			m_ResourceCount++;
		}

		// catalog of assets managed by this manager
		std::map<int, std::vector<std::shared_ptr<Resource>>> m_ResourcesByLevel;   
		std::map<std::string, std::shared_ptr<Resource>> m_ResourcesByName;   
		std::map<int,shared_ptr<Resource>> m_ResourcesByUuid;

		int m_CurrentLevel = 0;
		int m_ResourceCount;
		int m_CountLoadedResources = 0;
		int m_CountUnloadedResources = 0;
};


