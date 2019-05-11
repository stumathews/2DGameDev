#include "ResourceManager.h"
#include "tinyxml2.h"
#include <iostream>
#include <map>
#include "Resource.h"
using namespace tinyxml2;
using namespace std;

std::shared_ptr<Resource> ResourceManager::GetResource(string name)
{
	auto resource = m_resources[name];
	return resource;
}

void ResourceManager::LoadResources()
{	
	std::cout << "Loading all the resources from the Resources.xml ..." << std::endl;;
	
	XMLDocument resourceFile;

	// Load the resource file into memory
	resourceFile.LoadFile( "resources.xml" );		
	
	// Get first Asset
	auto pRoot = resourceFile.FirstChildElement();
	auto pFirstAssetElement = pRoot->FirstChildElement("Asset");
			
	RecordAsset(pFirstAssetElement);

	// Get remaining assets elements
	auto pNextAsset = pFirstAssetElement->NextSiblingElement();
	do
	{		
		RecordAsset(pNextAsset);
	}
	while ((pNextAsset = pNextAsset->NextSiblingElement()) != nullptr);
}

void ResourceManager::RecordAsset(tinyxml2::XMLElement * pNextAsset)
{
	auto nextAsset = GetAsset(pNextAsset);
	m_resources.insert(std::pair<string, std::shared_ptr<Resource>>(nextAsset->m_name, nextAsset));
}

std::shared_ptr<Resource> ResourceManager::GetAsset(XMLElement* element)
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
	auto asset = std::shared_ptr<Resource>(new Resource(uuid, name, path, type, level));

	std::cout << "Fetched Asset " << asset->m_name << "of type " << asset->m_type << std::endl;
	return asset;
}

