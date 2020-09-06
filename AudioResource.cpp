#include "AudioResource.h"
#include "asset.h"
#include <iostream>



AudioResource::AudioResource(int uid, string name, string path, string type, int scene) 
	: asset(uid, name, path, type, scene)
{
	
}

void AudioResource::load()
{

	m_IsLoaded = true;
}


void AudioResource::unload()
{
	m_IsLoaded = false;
}

AudioResource::~AudioResource()
{
}
