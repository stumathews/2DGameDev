#pragma once
#include <vector>
#include "GraphicsResource.h"
#include "tinyxml2.h"
#include <memory>



// Managest a set of graphics resources
class GraphicsResourceManager
{
public:
	 static GraphicsResourceManager& getInstance()
        {
            static GraphicsResourceManager instance;			
            return instance;
        }
		GraphicsResourceManager(GraphicsResourceManager const&)  = delete;
        void operator=(GraphicsResourceManager const&)  = delete;
		
		// Creates a graphics Resource
		std::shared_ptr<Resource> MakeResource(tinyxml2::XMLElement * assetXmlElement);
private:	

	std::vector<shared_ptr<GraphicsResource>> m_GraphicsResources;
	GraphicsResourceManager();
	~GraphicsResourceManager();

};

