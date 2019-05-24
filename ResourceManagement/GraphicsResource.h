#pragma once
#include "Resource.h"
#include <SDL.h>
#include <memory>
#include "Resource.h"

// A stand-alone graphics resource with backing storage that can load and unload itself
class GraphicsResource : public Resource
{
public:
	
	GraphicsResource(int uid, string name, string path, string type, int level);
	
	~GraphicsResource();

	// The binary data that will represent the resource once its loaded.
	SDL_Surface* m_Surface;

	//Load the resource onto the surface
	void VLoad();

	// a render resource can unload itself from memory
	void VUnload();
};

