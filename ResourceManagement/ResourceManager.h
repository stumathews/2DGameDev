#pragma once
#include <string>
#include <map>
#include "Resource.h"
#include "tinyxml2.h"
#include <memory>
#include "tinyxml2.h"
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
		std::shared_ptr<Resource> GetResource(string name);
    private:
        ResourceManager() 
		{
			// Load the resources on creation of the Resource Manager
			LoadResources();
		}
		void LoadResources();
		
		// Record the asset in the catalog of assets
		void RecordAsset(tinyxml2::XMLElement * assetElement);
		
		// Make Asset from Asset XMLElement
		std::shared_ptr<Resource> GetAsset(XMLElement* element);

		// catalog of assets managed by this manager
		std::map<std::string, std::shared_ptr<Resource>> m_resources;        
};


