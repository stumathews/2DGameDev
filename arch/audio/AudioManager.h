#pragma once
#include "../tinyxml2.h"
#include "AudioResource.h"
#include <memory>


class audio_manager
{
public:	
	// Creates an audio Resource
	std::shared_ptr<asset> create_asset(tinyxml2::XMLElement * asset_xml_element) const;
	static std::shared_ptr<audio_resource> to_resource(const std::shared_ptr<asset>& asset);
};

