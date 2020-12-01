#include "font_manager.h"
#include "tinyxml2.h"

std::shared_ptr<asset> font_manager::create_asset(tinyxml2::XMLElement* element)
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

	auto font = std::make_shared<font_resource>(uuid, name, path, type, scene);
	
	fonts.push_back(font);
	return font;
}
