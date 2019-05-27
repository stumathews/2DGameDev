#pragma once
#include "Resource.h"
#include <string>

class AudioResource : public Resource
{
public:
	AudioResource(int uid, string name, string path, string type, int scene);
	
	~AudioResource();
	//Load the resource onto the surface
	void VLoad();

	// a render resource can unload itself from memory
	void VUnload();
};

