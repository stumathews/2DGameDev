#include "AudioResource.h"
#include "Resource.h"
#include <iostream>



AudioResource::AudioResource(int uid, string name, string path, string type, int scene) 
	: Resource(uid, name, path, type, scene)
{
	
}

void AudioResource::VLoad()
{

	m_IsLoaded = true;
}


void AudioResource::VUnload()
{
	m_IsLoaded = false;
}

AudioResource::~AudioResource()
{
}
