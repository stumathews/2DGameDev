#pragma once
#include <SDL_image.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

template<typename T>
class Singleton
{
public:
	static Singleton<T>& GetInstance()
	{
		static Singleton<T> instance;		
		return instance;
	}
	Singleton(Singleton<T> const&) = delete;
	void operator=(Singleton<T> const&) = delete;
	T object;
private:	
	Singleton(){}
	~Singleton(){}
};

class GlobalConfig
{
public:
	GlobalConfig() {}
	Mix_Music *gMusic = NULL;
	Mix_Chunk *gScratch = NULL;
	Mix_Chunk *gHigh = NULL;
	Mix_Chunk *gMedium = NULL;
	Mix_Chunk *gLow = NULL;
	TTF_Font *font = NULL;
	bool verbose = true;
	bool printDebuggingText = false;
	bool use3dRengerManager = false;
};
