#pragma once

#include <SDL.h>
#include <vector>
#include <memory>
#include "EventManager.h"
#include "Actor.h"
#include <iostream>

struct GameWorldData 
{
	int x;
	int y;
	int w;
	int h;
	
	bool bGameDone;
	bool bNetworkGame;
	bool bCanRender;
		
	// List of game objects
	std::vector<std::shared_ptr<GameObjectBase>> actors;

	~GameWorldData()
	{
		std::cout << "Deleting game world data!" << std::endl;
	}
};

