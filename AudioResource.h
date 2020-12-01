#pragma once
#include "asset.h"
#include <string>
#include <SDL_mixer.h>

class audio_resource final : public asset
{
public:
	audio_resource(int uid, std::string name, const std::string path, const std::string type, int scene);

	// data
	Mix_Chunk *fx = nullptr;	
	Mix_Music* music = nullptr;

	Mix_Chunk *as_fx() const { return fx;}
	Mix_Music *as_music() const {return music;}

	
	//Load the resource onto the surface
	void load() override;
	bool is_fx() const;

	// a render resource can unload itself from memory
	bool unload() override;
};

