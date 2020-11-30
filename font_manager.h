#pragma once
#include <memory>
#include "asset.h"
#include <vector>
#include "font_resource.h"

namespace tinyxml2 {
	class XMLElement;
}

// Managers a set of font resources
class font_manager
{
public:
	 static font_manager& get_instance()
        {
            static font_manager instance;			
            return instance;
        }
		font_manager(font_manager const&)  = delete;
        void operator=(font_manager const&)  = delete;
		
		// Creates an audio Resource
		std::shared_ptr<asset> create_asset(tinyxml2::XMLElement * assetXmlElement);
private:	

	vector<shared_ptr<font_resource>> fonts;
	font_manager() = default;
	~font_manager() = default;

};
