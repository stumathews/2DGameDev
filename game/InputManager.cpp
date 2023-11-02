#include "InputManager.h"

void InputManager::Sample(const unsigned long deltaMs)
{
	accumulatedTimeMs += deltaMs;

	if(gameCommands == nullptr)
	{
		std::cout << "No game commands set on the input manager. No input will be sampled." << std::endl;
		return;
	}
	/*if(accumulatedTimeMs < static_cast<unsigned int>(1000) / GetSampleRatePerSec())
	{		
		return;
	}*/
	SDL_Event sdlEvent;
	while(SDL_PollEvent(&sdlEvent))
	{
		if (sdlEvent.type == SDL_QUIT)  { gameCommands->Quit(verbose); return; }		
	}

	const auto keyState = SDL_GetKeyboardState(nullptr);
			
	if (keyState[SDL_SCANCODE_UP] || keyState[SDL_SCANCODE_W]) { gameCommands->MoveUp(verbose); }
	if (keyState[SDL_SCANCODE_DOWN] || keyState[SDL_SCANCODE_S]) { gameCommands->MoveDown(verbose); }
	if (keyState[SDL_SCANCODE_LEFT] || keyState[SDL_SCANCODE_A]) { gameCommands->MoveLeft(verbose); }
	if (keyState[SDL_SCANCODE_RIGHT] || keyState[SDL_SCANCODE_D]) { gameCommands->MoveRight(verbose); }
	if (keyState[SDL_SCANCODE_Q] || keyState[SDL_SCANCODE_ESCAPE]) { gameCommands->Quit(verbose); }
	if (keyState[SDL_SCANCODE_R]) { gameCommands->ReloadSettings(verbose);  }
	if (keyState[SDL_SCANCODE_P]) { gameCommands->PingGameServer();  }
	if (keyState[SDL_SCANCODE_R]) { gameCommands->ReloadSettings(verbose);  }
	if (keyState[SDL_SCANCODE_R]) { gameCommands->ReloadSettings(verbose);  }
	if (keyState[SDL_SCANCODE_N]) { gameCommands->StartNetworkLevel();  }
	if (keyState[SDL_SCANCODE_SPACE]) { gameCommands->Fire(verbose);  }
	if (keyState[SDL_SCANCODE_0]) { gameCommands->ToggleMusic(false);  }
	
	
	accumulatedTimeMs = 0;
	sampleCount++;

}

int InputManager::GetSampleRatePerSec() 
{
	return samplePerSec;
}
