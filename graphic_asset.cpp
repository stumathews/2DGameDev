#include "graphic_asset.h"
#include <iostream>
#include <SDL_image.h>
#include "sdl_graphics_manager.h"
#include <SDL.h>
#include "Logger.h"

extern shared_ptr<sdl_graphics_manager> graphics_admin;

graphic_asset::graphic_asset(const int uid, string name, const string& path, const string& type, const int level, const bool is_animated)
							 : asset(uid, name, path, type, level),
                               key_frame_height(64),
                               key_frame_width(64),
                               is_animated(is_animated)
{
	/* ctor initializes member only */
}
graphic_asset::graphic_asset(const int uid, string name, const string& path, const string& type, const int level, const uint num_key_frames, const uint key_frame_height, const uint key_frame_width, const bool is_animated)
							 : asset(uid, name, path, type, level),
	                           num_key_frames(num_key_frames),
	                           key_frame_height(key_frame_height),
	                           key_frame_width(key_frame_width),
	                           is_animated(is_animated),
	                           view_port({ 0, 0, static_cast<int>(key_frame_width), static_cast<int>(key_frame_height) })
{
	/* ctor initializes member only */
};	



void graphic_asset::load()
{
	unload();
		  
	const auto loaded_surface = IMG_Load(path.c_str()); //Load image at specified path
	
	if(loaded_surface)
	{						
		texture = SDL_CreateTextureFromSurface(sdl_graphics_manager::get_instance().window_renderer, loaded_surface ); //Create texture from surface pixels
		
		SDL_FreeSurface(loaded_surface); //Get rid of old loaded surface
		
		if(texture)
			is_loaded = true;
	}
	else
	{
		logger::log_message(string("Unable to load image:") + path + string(" Error:") + IMG_GetError());
	}
}

void graphic_asset::unload()
{		
	SDL_DestroyTexture(texture);
	texture = nullptr;	
	is_loaded = false;
}

graphic_asset::~graphic_asset()
{
	if(is_loaded)
	{
		std::cout << "Destroying graphics resource: " << name << std::endl;
		graphic_asset::unload();
	}	
}
