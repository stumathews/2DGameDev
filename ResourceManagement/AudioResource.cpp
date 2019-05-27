#include "AudioResource.h"
#include "Resource.h"
#include <iostream>



AudioResource::AudioResource(int uid, string name, string path, string type, int scene) 
	: Resource(uid, name, path, type, scene)
{
	
}

void AudioResource::VLoad()
{
		std::cout << "Loading audio resource into memory: " << m_name << std::endl;
}


void AudioResource::VUnload()
{
		std::cout << "Unloading audio resource out of memory: " << m_name << std::endl;
}

AudioResource::~AudioResource()
{
}
