#pragma once
#include <vector>
#include "tinyxml2.h"
#include "AudioResource.h"
#include <memory>



// Managest a set of audio resources
class AudioResourceManager
{
public:
	 static AudioResourceManager& getInstance()
        {
            static AudioResourceManager instance;			
            return instance;
        }
		AudioResourceManager(AudioResourceManager const&)  = delete;
        void operator=(AudioResourceManager const&)  = delete;
		
		// Creates an audio Resource
		std::shared_ptr<Resource> MakeResource(tinyxml2::XMLElement * assetXmlElement);
private:	

	std::vector<shared_ptr<AudioResource>> m_AudioResources;
	AudioResourceManager();
	~AudioResourceManager();

};

