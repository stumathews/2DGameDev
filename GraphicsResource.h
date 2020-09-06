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
		: asset(uid, name, path, type, level), m_NumKeyFrames(0), m_KeyFrameHeight(64), m_KeyFrameWidth(64), m_bIsAnimated(isAnimated)
	{}
	GraphicsResource(int uid, string name, string path, string type, int level, uint numKeyFrames, uint keyFrameHeight, uint keyFrameWidth, bool isAnimated)
		: m_NumKeyFrames(numKeyFrames), m_KeyFrameHeight(keyFrameHeight), m_KeyFrameWidth(keyFrameWidth),  m_viewPort({ 0, 0, (int)keyFrameWidth, (int)keyFrameHeight }), m_bIsAnimated(isAnimated),
		asset(uid, name, path, type, level)
	{};	
	
	~GraphicsResource();

	// The binary data that will represent the resource once its loaded.
	SDL_Surface* m_Surface = nullptr;
	SDL_Texture* newTexture = nullptr;


	uint m_NumKeyFrames;
	uint m_KeyFrameHeight;
	uint m_KeyFrameWidth;
	bool m_bIsAnimated = false;
	SDL_Rect m_viewPort = {};

	//Load the resource onto the surface
	void load();

	// a render resource can unload itself from memory
	void unload();
};

