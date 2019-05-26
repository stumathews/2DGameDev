#pragma once
#include <vector>
#include "GraphicsResource.h"
#include "tinyxml2.h"
#include <memory>
#include "Actor.h"



// Manages the graphics in the project,
// specifically graphic-enabled objects like Actors and AnimatedActors.

class GraphicsManager
{
public:
	 static GraphicsManager& getInstance()
        {
            static GraphicsManager instance;			
            return instance;
        }
		GraphicsManager(GraphicsManager const&)  = delete;
        void operator=(GraphicsManager const&)  = delete;
		
		SDL_Renderer* m_MainWindow;
		// Initializes the mainWindow
		bool Init(unsigned width = 800, unsigned height=600, char* windowTitle=0);
		// Creates a graphics Resource
		std::shared_ptr<Resource> MakeResource(tinyxml2::XMLElement * assetXmlElement);
		
		// Draw all the Actors we know about onto the surface
		void DrawAllActors();
private:	

	std::vector<shared_ptr<Actor>> m_Actors;
	GraphicsManager();
	~GraphicsManager();

};

