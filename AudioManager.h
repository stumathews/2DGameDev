#pragma once
#include <vector>
#include "tinyxml2.h"
#include "AudioResource.h"
#include <memory>
#include "game_object.h"


// Managest a set of audio resources
class audio_manager
{
public:
	 static audio_manager& get_instance()
        {
            static audio_manager instance;			
            return instance;
        }
		audio_manager(audio_manager const&)  = delete;
        void operator=(audio_manager const&)  = delete;
		
		// Creates an audio Resource
		std::shared_ptr<asset> create_asset(tinyxml2::XMLElement * assetXmlElement);
		static shared_ptr<audio_resource> to_audio_resource(const shared_ptr<asset>& asset);
private:	

	std::vector<shared_ptr<audio_resource>> m_AudioResources;
	audio_manager();
	~audio_manager();

};

