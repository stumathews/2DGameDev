#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include "global_config.h"
#include <memory>
using namespace std;

extern shared_ptr<global_config> config;

class RectDebugging
{
public:
	static void printInRect(SDL_Renderer* renderer, std::string text, SDL_Rect* bounds)
	{
		SDL_Color color = { 255, 255, 255 };
		SDL_Surface * surface = TTF_RenderText_Solid(config->font, text.c_str(), color);
		SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);	
		SDL_RenderCopy(renderer, texture, NULL, bounds);
		SDL_DestroyTexture(texture);
		SDL_FreeSurface(surface);
	}

};

