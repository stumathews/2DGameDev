#include "AudioResource.h"
#include "asset.h"
#include <iostream>
#include <SDL_mixer.h>
#include "resource_manager.h"

extern shared_ptr<resource_manager> resource_admin;

audio_resource::audio_resource(int uid, string name, string path, string type, int scene) 
	: asset(uid, name, path, type, scene)
{
	
}

void audio_resource::load()
{
	if(is_fx())
		fx = Mix_LoadWAV(resource_admin->get_resource_by_name(name)->path.c_str());
	else
		music = Mix_LoadMUS(resource_admin->get_resource_by_name(name)->path.c_str());
	
	is_loaded = true;
}

bool audio_resource::is_fx() const
{
	return type == "fx";
}


void audio_resource::unload()
{
	if(is_fx() && fx != nullptr)
	{
		Mix_FreeChunk(fx);
		fx = nullptr;
		is_loaded = false;
	}
	else 
	{
		if(music != nullptr)
		{
			 Mix_FreeMusic( music );
			music = nullptr;
			is_loaded = false;
		}
	}
}

audio_resource::~audio_resource()
{
}
