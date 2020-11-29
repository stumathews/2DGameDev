#include "graphic_asset.h"
#include <iostream>
#include <SDL_image.h>
#include "sdl_graphics_manager.h"
#include <SDL.h>
#include "Logger.h"


//GraphicsResource::GraphicsResource(int uid, string name, string path, string type, int level, unsigned int numKeyFrames, unsigned int keyFrameHeight, unsigned int keyFrameWidth)
//	: GraphicsResource(uid, name, path, type, level)
//{	
//	m_NumKeyFrames = numKeyFrames;
//	m_KeyFrameHeight = keyFrameHeight;
//	m_KeyFrameWidth = keyFrameWidth;
//	m_bIsAnimated = m_NumKeyFrames > 0;
//
//}

graphic_asset::~graphic_asset()
{
	if(m_IsLoaded){
		std::cout << "Destroying graphics resource: " << m_name << std::endl;
	}
	graphic_asset::unload();
}

void graphic_asset::load()
{
	unload();
		  
	const auto loaded_surface = IMG_Load(m_path.c_str()); //Load image at specified path
	if(loaded_surface)
	{						
		texture = SDL_CreateTextureFromSurface(sdl_graphics_manager::get().window_renderer, loaded_surface ); //Create texture from surface pixels
		
		SDL_FreeSurface(loaded_surface); //Get rid of old loaded surface
		if(texture)
			m_IsLoaded = true;
	}
	else
	{
		logger::log_message(string("Unable to load image:") + m_path + string(" Error:") + IMG_GetError());
	}
}

void graphic_asset::unload()
{
	logger::log_message(string("unload() : ") + m_path );
	SDL_DestroyTexture(texture);
	texture = nullptr;	
	m_IsLoaded = false;
}
