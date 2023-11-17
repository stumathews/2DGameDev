#include "InputManager.h"

void InputManager::Sample(const unsigned long deltaMs)
{
	accumulatedTimeMs += deltaMs;

	if(gameCommands == nullptr)
	{
		std::cout << "No game commands set on the input manager. No input will be sampled." << std::endl;
		return;
	}

	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)  { gameCommands->Quit(verbose); return; }
		if (e.key.repeat == 0) // we ignore key repeats
		{
			switch(e.key.keysym.sym)
			{
				case SDLK_w:
			case SDLK_UP:
					gameCommands->MoveUp(verbose, e.type == SDL_KEYDOWN 
						? gamelib::ControllerMoveEvent::KeyState::Pressed
						: gamelib::ControllerMoveEvent::KeyState::Released);
	            break;
		        case SDLK_s:
		        case SDLK_DOWN:
					gameCommands->MoveDown(verbose, e.type == SDL_KEYDOWN 
						? gamelib::ControllerMoveEvent::KeyState::Pressed
						: gamelib::ControllerMoveEvent::KeyState::Released);
		            break;
		        case SDLK_a:
		        case SDLK_LEFT:
					gameCommands->MoveLeft(verbose, e.type == SDL_KEYDOWN 
						? gamelib::ControllerMoveEvent::KeyState::Pressed
						: gamelib::ControllerMoveEvent::KeyState::Released);
		            break;
		        case SDLK_d:
		        case SDLK_RIGHT:
					gameCommands->MoveRight(verbose, e.type == SDL_KEYDOWN 
						? gamelib::ControllerMoveEvent::KeyState::Pressed
						: gamelib::ControllerMoveEvent::KeyState::Released);
		            break;
            default: ;
			}
		}
	}

	const auto keyState = SDL_GetKeyboardState(nullptr);
	if (keyState[SDL_SCANCODE_Q] || keyState[SDL_SCANCODE_ESCAPE]) { gameCommands->Quit(verbose); }
	if (keyState[SDL_SCANCODE_R]) { gameCommands->ReloadSettings(verbose);  }
	if (keyState[SDL_SCANCODE_P]) { gameCommands->PingGameServer();  }
	if (keyState[SDL_SCANCODE_R]) { gameCommands->ReloadSettings(verbose); }
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
