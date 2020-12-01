#pragma once
#include <vector>
#include "tinyxml2.h"
#include "AudioResource.h"
#include <memory>
#include "game_object.h"


class audio_manager
{
public:	
	// Creates an audio Resource
	shared_ptr<asset> create_asset(tinyxml2::XMLElement * asset_xml_element);
	static shared_ptr<audio_resource> to_resource(const shared_ptr<asset>& asset);
private:	

	vector<shared_ptr<audio_resource>> m_AudioResources;
};

