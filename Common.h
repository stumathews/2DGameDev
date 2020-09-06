#pragma once

#include <SDL.h>
#include <vector>
#include <memory>
#include "EventManager.h"
#include "GameObject.h"

#include <iostream>

struct GameWorldData 
{
	int m_xPos;
	int m_yPos;
	int w;
	int h;
	
	bool bGameDone;
	bool bNetworkGame;
	bool bCanRender;
		
	// List of game objects
	std::vector<std::shared_ptr<game_object>> actors;

	~GameWorldData()
	{
		std::cout << "Deleting game world data!" << std::endl;
	}
};

