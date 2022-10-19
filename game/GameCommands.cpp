#include "GameCommands.h"
#include "common/common.h"
#include <memory>
#include "events/ControllerMoveEvent.h"
#include "events/SceneChangedEvent.h"
#include "scene/SceneManager.h"
#include <iostream>
#include <events/GameObjectEvent.h>
#include <events/NetworkTrafficRecievedEvent.h>
#include <sstream>
#include <net/NetworkManager.h>
#include <events/EventFactory.h>
#include <events/StartNetworkLevelEvent.h>
#include "LevelManager.h"

using namespace gamelib;
using namespace std;

GameCommands::GameCommands()
{
	verbose = SettingsManager::Get()->GetBool("global", "verbose");
	logCommands = SettingsManager::Get()->GetBool("global", "verbose");
	
	EventManager::Get()->SubscribeToEvent(EventType::NetworkPlayerJoined, this);
	EventManager::Get()->SubscribeToEvent(EventType::NetworkTrafficReceived, this);
}

void GameCommands::Fire(bool verbose)
{
	if(logCommands)
		Logger::Get()->LogThis("GameCommand: Fire", verbose);
	
	PlaySoundEffect(gamelib::AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo("scratch.wav"))->AsSoundEffect());
	
	EventManager::Get()->RaiseEvent(std::make_shared<gamelib::Event>(gamelib::EventType::Fire), this);
}

void GameCommands::MoveUp(bool verbose)
{
	if (logCommands)
		Logger::Get()->LogThis("GameCommand: MoveUp", verbose);
	
	EventManager::Get()->RaiseEvent(std::make_unique<gamelib::ControllerMoveEvent>(gamelib::Direction::Up), this);
}

void GameCommands::MoveDown(bool verbose)
{
	if (logCommands)
		Logger::Get()->LogThis("GameCommand: MoveDown", verbose);
	
	EventManager::Get()->RaiseEvent(std::make_unique<gamelib::ControllerMoveEvent>(gamelib::Direction::Down), this);
}

void GameCommands::MoveLeft(bool verbose)
{
	if (logCommands)
		Logger::Get()->LogThis("GameCommand: MoveLeft", verbose);

	EventManager::Get()->RaiseEvent(std::make_unique<gamelib::ControllerMoveEvent>(gamelib::Direction::Left), this);
}

void GameCommands::MoveRight(bool verbose)
{
	if (logCommands)
		Logger::Get()->LogThis("GameCommand: MoveRight", verbose);

	EventManager::Get()->RaiseEvent(std::make_unique<gamelib::ControllerMoveEvent>(gamelib::Direction::Right), this);
}

void GameCommands::PlaySoundEffect(Mix_Chunk* effect)
{
	if (logCommands)
		Logger::Get()->LogThis("GameCommand: PlaySoundEffect", verbose);

	AudioManager::Get()->Play(effect);
}

void GameCommands::RaiseChangedLevel(bool verbose, short newLevel)
{
	if (logCommands)
		Logger::Get()->LogThis("GameCommand: ChangeLevel", verbose);

	EventManager::Get()->RaiseEvent(std::make_unique<gamelib::SceneChangedEvent>(newLevel), this);
}

void GameCommands::ReloadSettings(bool verbose)
{
	if (logCommands)
		Logger::Get()->LogThis("GameCommand: ReloadSettings", verbose);

	SettingsManager::Get()->Reload();
	EventManager::Get()->RaiseEvent(make_shared<gamelib::Event>(gamelib::EventType::SettingsReloaded), this);
}

void GameCommands::GenerateNewLevel(bool verbose)
{
	if (logCommands)
		Logger::Get()->LogThis("GameCommand: GenerateNewLevel", verbose);

	RaiseChangedLevel(false, 1);
	EventManager::Get()->RaiseEvent(make_shared<gamelib::Event>(gamelib::EventType::GenerateNewLevel), this);
}

void GameCommands::LoadNewLevel(int level)
{
	switch (level)
	{
	case 1:
		LevelManager::Get()->CreateLevel(SettingsManager::Get()->GetString("global", "level1FileName"));
		break;
	case 2:
		LevelManager::Get()->CreateLevel(SettingsManager::Get()->GetString("global", "level2FileName"));
		break;
	case 3:
		LevelManager::Get()->CreateLevel(SettingsManager::Get()->GetString("global", "level3FileName"));
		break;
	case 4:
		LevelManager::Get()->CreateLevel(SettingsManager::Get()->GetString("global", "level4FileName"));
		break;
	}

	RaiseChangedLevel(verbose, level);
	
}

void GameCommands::ToggleMusic(bool verbose)
{	
	if (logCommands)
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
	if (logCommands)
		Logger::Get()->LogThis("GameCommand: Quitting", verbose);

	SceneManager::Get()->GetGameWorld().IsGameDone = 1;
}

void GameCommands::InvalidMove(bool verbose)
{
	if (logCommands)
		Logger::Get()->LogThis("GameCommand: Invalid move!", verbose);

	AudioManager::Get()->Play(AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo(SettingsManager::Get()->GetString("audio", "invalid_move")))->AsSoundEffect());

}

void GameCommands::FetchedPickup(bool verbose)
{
	if (logCommands)
		Logger::Get()->LogThis("GameCommand: FetchedPickup", verbose);

	AudioManager::Get()->Play(AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo(SettingsManager::Get()->GetString("audio", "fetched_pickup")))->AsSoundEffect());

	if(LevelManager::Get()->ReducePickupCount() == 0)
	{
		Logger::Get()->LogThis("All Pickups Collected Well Done!");
	}
}

void GameCommands::StartNetworkLevel()
{
	// This only works on the Game server
	if(!SceneManager::Get()->GetGameWorld().IsNetworkGame)
	{
		return;
	}

	if (logCommands)
		Logger::Get()->LogThis("GameCommand: StartNetworkLevel", verbose);

	// Ask the LevelManager to prepare a level description and pass that to StartNetworkLevelEvent
	// and let that propogate to all players

	EventManager::Get()->RaiseEvent(std::make_unique<gamelib::StartNetworkLevelEvent>(1), this);
}

void GameCommands::PingGameServer()
{
	NetworkManager::Get()->PingGameServer();
}

std::vector<std::shared_ptr<Event>> GameCommands::HandleEvent(std::shared_ptr<Event> event)
{
	switch (event->type)
	{
	case EventType::NetworkPlayerJoined:
		Logger::Get()->LogThis("---------------------------Network Player joined");
		break;
	case EventType::NetworkTrafficReceived:
		auto networkPlayerTrafficReceivedEvent = dynamic_pointer_cast<gamelib::NetworkTrafficRecievedEvent>(event);
		std::stringstream message;
		message << "---------------------------Network traffic received: " 
			    << networkPlayerTrafficReceivedEvent->Identifier << " Bytes received: "
			    << networkPlayerTrafficReceivedEvent->bytesReceived << " Message: " << networkPlayerTrafficReceivedEvent->Message;
		    
		Logger::Get()->LogThis(message.str());
		break;
	}
	// Dont currently handle any events yet
	return std::vector<std::shared_ptr<Event>>();
}

std::string GameCommands::GetSubscriberName() 
{
	return "GameCommands";
}
