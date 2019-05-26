#include "AudioManager.h"
#include <vector>
#include <memory>
#include "Resource.h"



AudioManager::AudioManager()
{
}


AudioManager::~AudioManager()
{
}

std::shared_ptr<Resource> AudioManager::MakeResource(tinyxml2::XMLElement * element)
{
	int uuid;
	const char* type;
	const char* path;
	const char* name;
	int level;

	element->QueryIntAttribute("uid", &uuid);
	element->QueryStringAttribute("type", &type);
	element->QueryStringAttribute("filename", &path);
	element->QueryStringAttribute("name", &name);
	element->QueryIntAttribute("level", &level);

	// Read anything specific to audio in the element here...

	auto audioResource = std::shared_ptr<AudioResource>(new AudioResource(uuid, name, path, type, level));
	
	m_AudioResources.push_back(audioResource);
	return audioResource;
}
