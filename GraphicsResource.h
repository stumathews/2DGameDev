#pragma once
#include "asset.h"
#include <SDL.h>
#include <memory>
#include "asset.h"
#include "TypeAliases.h"
#include "TypeAliases.h"

// A stand-alone graphics resource with backing storage that can load and unload itself
class GraphicsResource : public asset
{
public:
		
	GraphicsResource(int uid, string name, string path, string type, int level, bool isAnimated) 
		: asset(uid, name, path, type, level), num_key_frames(0), key_frame_height(64), key_frame_width(64), m_bIsAnimated(isAnimated)
	{}
	GraphicsResource(int uid, string name, string path, string type, int level, uint numKeyFrames, uint keyFrameHeight, uint keyFrameWidth, bool isAnimated)
		: num_key_frames(numKeyFrames), key_frame_height(keyFrameHeight), key_frame_width(keyFrameWidth),  m_viewPort({ 0, 0, (int)keyFrameWidth, (int)keyFrameHeight }), m_bIsAnimated(isAnimated),
		asset(uid, name, path, type, level)
	{};	
	
	~GraphicsResource();

	// The binary data that will represent the resource once its loaded.
	SDL_Surface* m_Surface = nullptr;
	SDL_Texture* newTexture = nullptr;


	uint num_key_frames;
	uint key_frame_height;
	uint key_frame_width;
	bool m_bIsAnimated = false;
	SDL_Rect m_viewPort = {};

	//Load the resource onto the surface
	void load();

	// a render resource can unload itself from memory
	void unload();
};

