#include "AudioManager.h"
#include <vector>
#include <memory>
#include "asset.h"



AudioManager::AudioManager()
= default;


AudioManager::~AudioManager()
= default;

std::shared_ptr<asset> AudioManager::make_resource_spec(tinyxml2::XMLElement * element)
{
	int uuid;
	const char* type;
	const char* path;
	const char* name;
	int scene;

	element->QueryIntAttribute("uid", &uuid);
	element->QueryStringAttribute("type", &type);
	element->QueryStringAttribute("filename", &path);
	element->QueryStringAttribute("name", &name);
	element->QueryIntAttribute("scene", &scene);

	// Read anything specific to audio in the element here...

	auto audioResource = std::shared_ptr<AudioResource>(new AudioResource(uuid, name, path, type, scene));
	
	m_AudioResources.push_back(audioResource);
	return audioResource;
}
