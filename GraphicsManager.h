#pragma once
#include <vector>
#include "GraphicsResource.h"
#include "tinyxml2.h"
#include <memory>
#include "GameObjectBase.h"
#include "SDL.h"



// Manages the graphics in the project,
// specifically graphic-enabled objects like Actors and AnimatedActors.

class SDLGraphicsManager
{
public:
	 static SDLGraphicsManager& GetInstance()
        {
            static SDLGraphicsManager instance;			
            return instance;
        }
		SDLGraphicsManager(SDLGraphicsManager const&)  = delete;
        void operator=(SDLGraphicsManager const&)  = delete;
		
		SDL_Window* m_Window;
		SDL_Renderer* m_Renderer;
		SDL_Surface* m_WindowSurface;
		
		/* Initializes the graphics system
		* Creates Main Window
		* Sets up the window surface and renderer for drawing	
		*/
		bool Initialize(unsigned int width = 800, unsigned int height=600, const char* windowTitle=0);
		// Creates a graphics Resource
		std::shared_ptr<Resource> MakeResource(tinyxml2::XMLElement * assetXmlElement);
		
		// Draw all the Actors we know about onto the surface
		void DrawAllActors();

		void DrawCurrentScene();
		unsigned int GetScreenWidth() { return  m_ScreenWidth;}
		unsigned int GetScreenhEIGHT() { return  m_ScreenHeight;}
private:	

	std::vector<shared_ptr<GameObjectBase>> m_Actors;
	SDLGraphicsManager(){}
	~SDLGraphicsManager();
	unsigned int m_ScreenWidth;
	unsigned int m_ScreenHeight;

};

