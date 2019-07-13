#pragma once
#include <vector>
#include "GraphicsResource.h"
#include "tinyxml2.h"
#include <memory>
#include "Actor.h"
#include "SDL.h"



// Manages the graphics in the project,
// specifically graphic-enabled objects like Actors and AnimatedActors.

class GraphicsManager
{
public:
	 static GraphicsManager& GetInstance()
        {
            static GraphicsManager instance;			
            return instance;
        }
		GraphicsManager(GraphicsManager const&)  = delete;
        void operator=(GraphicsManager const&)  = delete;
		
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

	std::vector<shared_ptr<ActorBase>> m_Actors;
	GraphicsManager(){}
	~GraphicsManager();
	unsigned int m_ScreenWidth;
	unsigned int m_ScreenHeight;

};

