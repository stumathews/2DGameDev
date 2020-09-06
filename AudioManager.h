#pragma once
#include <vector>
#include "tinyxml2.h"
#include "AudioResource.h"
#include <memory>



// Managest a set of audio resources
class AudioManager
{
public:
	 static AudioManager& GetInstance()
        {
            static AudioManager instance;			
            return instance;
        }
		AudioManager(AudioManager const&)  = delete;
        void operator=(AudioManager const&)  = delete;
		
		// Creates an audio Resource
		std::shared_ptr<asset> make_resource_spec(tinyxml2::XMLElement * assetXmlElement);
private:	

	std::vector<shared_ptr<AudioResource>> m_AudioResources;
	AudioManager();
	~AudioManager();

};

