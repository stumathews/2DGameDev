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
	
	// 20 times a second = 50 milliseconds
	// 1 second is 20*50 = 1000 milliseconds
	static const int TICK_TIME_MS = 50;
	static const int squareWidth = 30;
	static const int MaxLoops = 4;
	static const int ScreenWidth = 800;
	static const int ScreenHeight = 600;
	static const int moveInterval = 1;
	Mix_Music *gMusic = nullptr;
	Mix_Chunk *gScratch = nullptr;
	Mix_Chunk *gHigh = nullptr;
	Mix_Chunk *gMedium = nullptr;
	Mix_Chunk *gLow = nullptr;
	TTF_Font *font = nullptr;
	bool verbose = true;
	bool printDebuggingText = false;
	bool use3dRengerManager = false;
};
