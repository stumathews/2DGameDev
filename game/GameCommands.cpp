#include "GameCommands.h"
#include "common/common.h"
#include <memory>
#include "events/ControllerMoveEvent.h"
#include "events/SceneChangedEvent.h"
#include "scene/SceneManager.h"
#include <iostream>
#include <events/GameObjectEvent.h>

using namespace gamelib;
using namespace std;

GameCommands::GameCommands()
{
	this->verbose = false;
	
	EventManager::Get()->SubscribeToEvent(EventType::NetworkPlayerJoined, this);
}

void GameCommands::Fire(bool verbose)
{
	Logger::Get()->LogThis("GameCommand: Fire", true);
	
	PlaySoundEffect(gamelib::AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo("scratch.wav"))->AsSoundEffect());
	
	EventManager::Get()->RaiseEvent(std::make_shared<gamelib::Event>(gamelib::EventType::Fire), this);
}

void GameCommands::MoveUp(bool verbose)
{
	Logger::Get()->LogThis("GameCommand: MoveUp", true);
	EventManager::Get()->RaiseEvent(std::make_unique<gamelib::ControllerMoveEvent>(gamelib::Direction::Up), this);
}

void GameCommands::MoveDown(bool verbose)
{
	Logger::Get()->LogThis("GameCommand: MoveDown", true);
	EventManager::Get()->RaiseEvent(std::make_unique<gamelib::ControllerMoveEvent>(gamelib::Direction::Down), this);
}

void GameCommands::MoveLeft(bool verbose)
{
	Logger::Get()->LogThis("GameCommand: MoveLeft", true);
	EventManager::Get()->RaiseEvent(std::make_unique<gamelib::ControllerMoveEvent>(gamelib::Direction::Left), this);
}

void GameCommands::MoveRight(bool verbose)
{
	Logger::Get()->LogThis("GameCommand: MoveRight", true);
	EventManager::Get()->RaiseEvent(std::make_unique<gamelib::ControllerMoveEvent>(gamelib::Direction::Right), this);
}

void GameCommands::PlaySoundEffect(Mix_Chunk* effect)
{
	Logger::Get()->LogThis("GameCommand: PlaySoundEffect", true);
	AudioManager::Get()->Play(effect);
}

void GameCommands::ChangeLevel(bool verbose, short newLevel)
{
	Logger::Get()->LogThis("GameCommand: ChangeLevel", verbose);
	EventManager::Get()->RaiseEvent(std::make_unique<gamelib::SceneChangedEvent>(newLevel), this);
}

void GameCommands::ReloadSettings(bool verbose)
{
	Logger::Get()->LogThis("GameCommand: ReloadSettings", verbose);
	SettingsManager::Get()->Reload();
	EventManager::Get()->RaiseEvent(make_shared<gamelib::Event>(gamelib::EventType::SettingsReloaded), this);
}

void GameCommands::GenerateNewLevel(bool verbose)
{
	Logger::Get()->LogThis("GameCommand: GenerateNewLevel", verbose);
	EventManager::Get()->RaiseEvent(make_shared<gamelib::Event>(gamelib::EventType::GenerateNewLevel), this);
}

void GameCommands::ToggleMusic(bool verbose)
{	
	Logger::Get()->LogThis("GameCommand: ToggleMusic", verbose);
	if (!Mix_PlayingMusic())
	{
		AudioManager::Get()->Play(gamelib::AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo("MainTheme.wav"))->AsMusic());
	}
	else
	{
		if (Mix_PausedMusic() == 1)
		{
			Mix_ResumeMusic();
		}
		else
		{
			Mix_PauseMusic();
		}
	}
}

void GameCommands::Quit(bool verbose)
{	
	Logger::Get()->LogThis("GameCommand: Quitting", verbose);
	SceneManager::Get()->GetGameWorld().IsGameDone = 1;
}

void GameCommands::InvalidMove(bool verbose)
{
	Logger::Get()->LogThis("GameCommand: Invalid move!", verbose);
	AudioManager::Get()->Play(AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo(SettingsManager::Get()->GetString("audio", "invalid_move")))->AsSoundEffect());

}

void GameCommands::FetchedPickup(bool verbose)
{
	Logger::Get()->LogThis("GameCommand: FetchedPickup", verbose);
	AudioManager::Get()->Play(AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo(SettingsManager::Get()->GetString("audio", "fetched_pickup")))->AsSoundEffect());
}

std::vector<std::shared_ptr<Event>> GameCommands::HandleEvent(std::shared_ptr<Event> event)
{
	switch (event->type)
	{
	case EventType::NetworkPlayerJoined:
		Logger::Get()->LogThis("---------------------------Network Player joined");
		break;
	}
	// Dont currently handle any events yet
	return std::vector<std::shared_ptr<Event>>();
}

std::string GameCommands::GetSubscriberName() 
{
	return "GameCommands";
}
