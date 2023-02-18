#include "GameCommands.h"
#include "common/common.h"
#include <memory>
#include "events/ControllerMoveEvent.h"
#include "events/SceneChangedEvent.h"
#include "scene/SceneManager.h"
#include <events/NetworkTrafficReceivedEvent.h>
#include <sstream>
#include <net/NetworkManager.h>
#include <events/StartNetworkLevelEvent.h>
#include "LevelManager.h"
#include <GameData.h>

#include "EventNumber.h"
#include "events/NetworkPlayerJoinedEvent.h"

using namespace gamelib;
using namespace std;

GameCommands::GameCommands()
{
	_verbose = SettingsManager::Get()->GetBool("global", "verbose");
	logCommands = SettingsManager::Get()->GetBool("global", "verbose");
	
	EventManager::Get()->SubscribeToEvent(NetworkPlayerJoinedEventId, this);
	EventManager::Get()->SubscribeToEvent(NetworkTrafficReceivedEventId, this);
}

void GameCommands::Fire(const bool verbose)
{
	if (logCommands) { Logger::Get()->LogThis("GameCommand: Fire", verbose); }	
	PlaySoundEffect(AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo("scratch.wav"))->AsSoundEffect());	
	EventManager::Get()->RaiseEvent(std::make_shared<Event>(FireEventId), this);
}

void GameCommands::MoveUp(const bool verbose)
{
	if (logCommands) { Logger::Get()->LogThis("GameCommand: MoveUp", verbose); }	
	EventManager::Get()->RaiseEvent(std::make_unique<ControllerMoveEvent>(Direction::Up), this);
}

void GameCommands::MoveDown(const bool verbose)
{
	if (logCommands) { Logger::Get()->LogThis("GameCommand: MoveDown", verbose); }	
	EventManager::Get()->RaiseEvent(std::make_unique<ControllerMoveEvent>(Direction::Down), this);
}

void GameCommands::MoveLeft(const bool verbose)
{
	if (logCommands) { Logger::Get()->LogThis("GameCommand: MoveLeft", verbose); }
	EventManager::Get()->RaiseEvent(std::make_unique<ControllerMoveEvent>(Direction::Left), this);
}

void GameCommands::MoveRight(const bool verbose)
{
	if (logCommands) { Logger::Get()->LogThis("GameCommand: MoveRight", verbose); }
	EventManager::Get()->RaiseEvent(std::make_unique<ControllerMoveEvent>(Direction::Right), this);
}

void GameCommands::PlaySoundEffect(Mix_Chunk* effect) const
{
	if (logCommands) { Logger::Get()->LogThis("GameCommand: PlaySoundEffect", _verbose); }
	AudioManager::Get()->Play(effect);
}

void GameCommands::RaiseChangedLevel(const bool verbose, short newLevel)
{
	if (logCommands) { Logger::Get()->LogThis("GameCommand: ChangeLevel", verbose); }
	EventManager::Get()->RaiseEvent(std::make_unique<SceneChangedEvent>(newLevel), this);
}

void GameCommands::ReloadSettings(const bool verbose)
{
	if (logCommands) { Logger::Get()->LogThis("GameCommand: ReloadSettings", verbose); }

	SettingsManager::Get()->Reload();
	EventManager::Get()->RaiseEvent(make_shared<Event>(SettingsReloadedEventId), this);
}

void GameCommands::LoadNewLevel(const int level)
{
	switch (level)
	{
		case 1: { LevelManager::Get()->CreateLevel(SettingsManager::Get()->GetString("global", "level1FileName")); } break;
		case 2: { LevelManager::Get()->CreateLevel(SettingsManager::Get()->GetString("global", "level2FileName")); } break;
		case 3: { LevelManager::Get()->CreateLevel(SettingsManager::Get()->GetString("global", "level3FileName")); } break;
		case 4: { LevelManager::Get()->CreateLevel(SettingsManager::Get()->GetString("global", "level4FileName")); } break;
		default: { LevelManager::Get()->CreateAutoLevel(); } break;
	}

	RaiseChangedLevel(_verbose, static_cast<short>(level));	
}

void GameCommands::ToggleMusic(const bool verbose) const
{	
	if (logCommands) { Logger::Get()->LogThis("GameCommand: ToggleMusic", verbose); }

	if (!Mix_PlayingMusic())
	{
		AudioManager::Get()->Play(AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo("MainTheme.wav"))->AsMusic());
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

void GameCommands::Quit(const bool verbose) const
{	
	if (logCommands) { Logger::Get()->LogThis("GameCommand: Quitting", verbose); }

	GameData::Get()->IsGameDone = true;
}

void GameCommands::InvalidMove(const bool verbose) const
{
	if (logCommands) { Logger::Get()->LogThis("GameCommand: Invalid move!", verbose); }
}

void GameCommands::FetchedPickup(const bool verbose) const
{
	if (logCommands) { Logger::Get()->LogThis("GameCommand: FetchedPickup", verbose); }

	AudioManager::Get()->Play(AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo(SettingsManager::Get()->GetString("audio", "fetched_pickup")))->AsSoundEffect());
}

void GameCommands::StartNetworkLevel()
{
	// This only works on the Game server
	if(!GameData::Get()->IsNetworkGame)
	{
		return;
	}

	if (logCommands) { Logger::Get()->LogThis("GameCommand: StartNetworkLevel", _verbose); }

	// Ask the LevelManager to prepare a level description and pass that to StartNetworkLevelEvent
	// and let that propagate to all players

	EventManager::Get()->RaiseEvent(std::make_unique<StartNetworkLevelEvent>(1), this);
}

void GameCommands::PingGameServer() { NetworkManager::Get()->PingGameServer(); }

std::vector<std::shared_ptr<Event>> GameCommands::HandleEvent(const std::shared_ptr<Event> evt, unsigned long deltaMs)
{
	if(evt->Id.Id == NetworkPlayerJoinedEventId.Id) { Logger::Get()->LogThis("--------------------------- Network Player joined");}
	if(evt->Id.Id == NetworkTrafficReceivedEventId.Id)
	{
		const auto networkPlayerTrafficReceivedEvent = dynamic_pointer_cast<NetworkTrafficReceivedEvent>(evt);
		std::stringstream message;
		message << "--------------------------- Network traffic received: " 
			    << networkPlayerTrafficReceivedEvent->Identifier << " Bytes received: "
			    << networkPlayerTrafficReceivedEvent->BytesReceived << " Message: " << networkPlayerTrafficReceivedEvent->Message;
		    
		Logger::Get()->LogThis(message.str());
	}

	// Don't currently handle any events yet
	return {};
}