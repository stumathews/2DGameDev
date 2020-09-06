#pragma once
#include "asset.h"
#include <string>

class AudioResource : public asset
{
public:
	AudioResource(int uid, string name, string path, string type, int scene);
	
	~AudioResource();
	//Load the resource onto the surface
	void load();

	// a render resource can unload itself from memory
	void unload();
};

