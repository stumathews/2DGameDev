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
	_verbose = SettingsManager::Bool("global", "verbose");
	logCommands = SettingsManager::Bool("global", "verbose");

	// The Game Commands subscribe to certain event too
	EventManager::Get()->SubscribeToEvent(NetworkPlayerJoinedEventId, this);
	EventManager::Get()->SubscribeToEvent(NetworkTrafficReceivedEventId, this);
}

void GameCommands::Fire(const bool verbose)
{
	// Log it
	if (logCommands) { Logger::Get()->LogThis("GameCommand: Fire", verbose); }

	// Sound it
	PlaySoundEffect(AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo("scratch.wav")));

	// Send event to do it
	EventManager::Get()->RaiseEvent(std::make_shared<Event>(FireEventId), this);
}

void GameCommands::MoveUp(const bool verbose)
{
	// Log it
	if (logCommands) { Logger::Get()->LogThis("GameCommand: MoveUp", verbose); }

	// Send even to do it
	EventManager::Get()->RaiseEvent(std::make_unique<ControllerMoveEvent>(Direction::Up), this);
}

void GameCommands::MoveDown(const bool verbose)
{
	// Log it
	if (logCommands) { Logger::Get()->LogThis("GameCommand: MoveDown", verbose); }

	// Send event to do it
	EventManager::Get()->RaiseEvent(std::make_unique<ControllerMoveEvent>(Direction::Down), this);
}

void GameCommands::MoveLeft(const bool verbose)
{
	// Log it
	if (logCommands) { Logger::Get()->LogThis("GameCommand: MoveLeft", verbose); }

	// Send event to do it
	EventManager::Get()->RaiseEvent(std::make_unique<ControllerMoveEvent>(Direction::Left), this);
}

void GameCommands::MoveRight(const bool verbose)
{
	// Log it
	if (logCommands) { Logger::Get()->LogThis("GameCommand: MoveRight", verbose); }

	// Send event to do it
	EventManager::Get()->RaiseEvent(std::make_unique<ControllerMoveEvent>(Direction::Right), this);
}

void GameCommands::PlaySoundEffect(const shared_ptr<AudioAsset>& effect) const
{
	// Log it
	if (logCommands) { Logger::Get()->LogThis("GameCommand: PlaySoundEffect", _verbose); }

	// do it
	AudioManager::Get()->Play(effect);
}

void GameCommands::RaiseChangedLevel(const bool verbose, short newLevel)
{
	// Log it
	if (logCommands) { Logger::Get()->LogThis("GameCommand: ChangeLevel", verbose); }

	// Send event to do it
	EventManager::Get()->RaiseEvent(std::make_unique<SceneChangedEvent>(newLevel), this);
}

void GameCommands::ReloadSettings(const bool verbose)
{
	// Log it
	if (logCommands) { Logger::Get()->LogThis("GameCommand: ReloadSettings", verbose); }

	// Do it
	SettingsManager::Get()->Reload();

	// Send event to do it elsewhere
	EventManager::Get()->RaiseEvent(make_shared<Event>(SettingsReloadedEventId), this);
}

void GameCommands::LoadNewLevel(const int level)
{
	switch (level)
	{
		case 1: { LevelManager::Get()->CreateLevel(SettingsManager::String("global", "level1FileName")); } break;
		case 2: { LevelManager::Get()->CreateLevel(SettingsManager::String("global", "level2FileName")); } break;
		case 3: { LevelManager::Get()->CreateLevel(SettingsManager::String("global", "level3FileName")); } break;
		case 4: { LevelManager::Get()->CreateLevel(SettingsManager::String("global", "level4FileName")); } break;
		case 5: { LevelManager::Get()->CreateLevel(SettingsManager::String("global", "level5FileName")); } break;
		default: { LevelManager::Get()->CreateAutoLevel(); } break;
	}

	// Send event to change level to selected level
	RaiseChangedLevel(_verbose, static_cast<short>(level));	
}

void GameCommands::ToggleMusic(const bool verbose) const
{
	// Log it
	if (logCommands) { Logger::Get()->LogThis("GameCommand: ToggleMusic", verbose); }

	if (!Mix_PlayingMusic() && !Mix_PausedMusic())
	{
		// We always plat level 4's music if no music is playing
		AudioManager::Get()->Play(AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo("LevelMusic4")));
	}
		
	if (Mix_PausedMusic() == 1) 
	{ 
		Mix_ResumeMusic();
	}
	else
	{
		Mix_PauseMusic();
	}	
}

void GameCommands::Quit(const bool verbose) const
{
	// Log it
	if (logCommands) { Logger::Get()->LogThis("GameCommand: Quitting", verbose); }

	// Do it
	GameData::Get()->IsGameDone = true;
}

void GameCommands::InvalidMove(const bool verbose) const
{
	// Log it
	if (logCommands) { Logger::Get()->LogThis("GameCommand: Invalid move!", verbose); }
}

void GameCommands::FetchedPickup(const bool verbose) const
{
	// Log it
	if (logCommands) { Logger::Get()->LogThis("GameCommand: FetchedPickup", verbose); }

	AudioManager::Get()->Play(ResourceManager::Get()->GetAssetInfo(SettingsManager::String("audio", "fetched_pickup")));
}

void GameCommands::StartNetworkLevel()
{
	// This only works on the Game server
	if(GameData::Get()->IsMultiPlayerGame())
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
	if(evt->Id.PrimaryId == NetworkPlayerJoinedEventId.PrimaryId) { Logger::Get()->LogThis("--------------------------- Network Player joined");}
	if(evt->Id.PrimaryId == NetworkTrafficReceivedEventId.PrimaryId)
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