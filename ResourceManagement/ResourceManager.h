#pragma once
#include <string>
#include <map>
#include "Resource.h"
#include "tinyxml2.h"
#include <memory>
#include "tinyxml2.h"
#include "GraphicsResourceManager.h"
using namespace std;
using namespace tinyxml2;


class ResourceManager
{
    public:
        static ResourceManager& getInstance()
        {
            static ResourceManager instance;			
            return instance;
        }
		ResourceManager(ResourceManager const&)  = delete;
        void operator=(ResourceManager const&)  = delete;
		std::shared_ptr<Resource> GetResourceByName(string name);
		int GetResourceCount(){ return m_ResourceCount; }
		// Load all the resources in the current level and unload those not in the current level
		void SetCurrentLevel(int level);
    private:
		
		ResourceManager()
		{
			m_ResourceCount = 0;
			// Load the resources on creation of the Resource Manager
			ReadInResources();
		}
		void ReadInResources();
				
		void StoreResource(std::shared_ptr<Resource> resource)
		{	
			m_ResourcesByLevel[resource->m_level].push_back(resource);			

			m_ResourcesByName.insert(std::pair<string, std::shared_ptr<Resource>>(resource->m_name, resource));	
			m_ResourceCount++;
		}

		// catalog of assets managed by this manager
		std::map<int, std::vector<std::shared_ptr<Resource>>> m_ResourcesByLevel;   
		std::map<std::string, std::shared_ptr<Resource>> m_ResourcesByName;   

		int m_ResourceCount;
};


