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
	GraphicsResource(int uid, string name, string path, string type, int level, 
					 unsigned int numKeyFrames, unsigned int keyFrameHeight, unsigned int keyFrameWidth);	
	
	~GraphicsResource();

	// The binary data that will represent the resource once its loaded.
	SDL_Surface* m_Surface;

	unsigned int m_NumKeyFrames;
	unsigned int m_KeyFrameHeight;
	unsigned int m_KeyFrameWidth;
	bool m_bIsAnimated = false;

	//Load the resource onto the surface
	void VLoad();

	// a render resource can unload itself from memory
	void VUnload();
};

