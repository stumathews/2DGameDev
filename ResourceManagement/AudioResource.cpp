#include "AudioResource.h"
#include "Resource.h"



AudioResource::AudioResource(int uid, string name, string path, string type, int level) 
	: Resource(uid, name, path, type, level)
{
}

void AudioResource::VLoad()
{
}


void AudioResource::VUnload()
{
}

AudioResource::~AudioResource()
{
}
