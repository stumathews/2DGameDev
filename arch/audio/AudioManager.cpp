#include "AudioManager.h"
#include <memory>

#include "common/Common.h"

using namespace std;

std::shared_ptr<asset> audio_manager::create_asset(tinyxml2::XMLElement * element) const
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
	// ... Read anything specific to audio in the element here...	
	
	auto audio = std::make_shared<audio_resource>(uuid, std::string(name), string(path), string(type), scene);

	return audio;
}

shared_ptr<audio_resource> audio_manager::to_resource(const shared_ptr<asset>& asset)
{	
	return as_resource<audio_resource>(asset);
}
