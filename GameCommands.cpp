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
#include "utils/Utils.h"
#include "2DGameDevLib/GameDataManager.h"

using namespace gamelib;
using namespace std;

GameCommands::GameCommands()
{
	verbose = SettingsManager::Bool("global", "verbose");
	logCommands = SettingsManager::Bool("global", "verbose");
	
	//EventManager::Get()->SubscribeToEvent(PlayerMovedEventTypeEventId, this);
}

std::string GameCommands::GetSubscriberName()
{
	return "GameCommands";
}

void GameCommands::Fire(const bool beVerbose)
{
	if (logCommands) { Logger::Get()->LogThis("GameCommand: Fire", beVerbose); }
	
	PlaySoundEffect(AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo("scratch.wav")));
	
	EventManager::Get()->RaiseEvent(EventFactory::Get()->CreateGenericEvent(FireEventId, GetSubscriberName()), this);
}

void GameCommands::MoveUp(const bool beVerbose, const ControllerMoveEvent::KeyState keyState)
{
	if (logCommands) { Logger::Get()->LogThis("GameCommand: MoveUp", beVerbose); }

	Move(Direction::Up, keyState);
}

void GameCommands::MoveDown(const bool beVerbose, const ControllerMoveEvent::KeyState keyState)
{
	if (logCommands) { Logger::Get()->LogThis("GameCommand: MoveDown", beVerbose); }

	Move(Direction::Down, keyState);
}

void GameCommands::MoveLeft(const bool beVerbose, const ControllerMoveEvent::KeyState keyState)
{
	if (logCommands) { Logger::Get()->LogThis("GameCommand: MoveLeft", beVerbose); }

	Move(Direction::Left, keyState);
}

void GameCommands::MoveRight(const bool beVerbose, const ControllerMoveEvent::KeyState keyState)
{
	if (logCommands) { Logger::Get()->LogThis("GameCommand: MoveRight", beVerbose); }

	Move(Direction::Right, keyState);
}

void GameCommands::Move(const Direction direction, ControllerMoveEvent::KeyState keyState = ControllerMoveEvent::KeyState::Pressed)
{
	
	switch(direction)
	{
		case Direction::Up: EventManager::Get()->RaiseEvent(EventFactory::Get()->CreateControllerMoveEvent(Direction::Up, keyState), this); 	break;
		case Direction::Down: EventManager::Get()->RaiseEvent(EventFactory::Get()->CreateControllerMoveEvent(Direction::Down, keyState), this); break;
		case Direction::Left: EventManager::Get()->RaiseEvent(EventFactory::Get()->CreateControllerMoveEvent(Direction::Left, keyState), this); 	break;
		case Direction::Right: EventManager::Get()->RaiseEvent(EventFactory::Get()->CreateControllerMoveEvent(Direction::Right, keyState), this); break;
		case Direction::None: THROW(12, "Unknown direction", "GameCommands");
	}
}

void GameCommands::PlaySoundEffect(const shared_ptr<AudioAsset>& effect) const
{
	if (logCommands) { Logger::Get()->LogThis("GameCommand: PlaySoundEffect", verbose); }

	AudioManager::Get()->Play(effect);
}

void GameCommands::RaiseChangedLevel(const bool beVerbose, const short newLevel)
{
	if (logCommands) { Logger::Get()->LogThis("GameCommand: ChangeLevel", beVerbose); }

	EventManager::Get()->RaiseEvent(EventFactory::Get()->CreateSceneChangedEventEvent(newLevel), this);
}

void GameCommands::ReloadSettings(const bool beVerbose)
{
	if (logCommands) { Logger::Get()->LogThis("GameCommand: ReloadSettings", beVerbose); }

	SettingsManager::Get()->Reload();

	EventManager::Get()->RaiseEvent(EventFactory::Get()->CreateGenericEvent(SettingsReloadedEventId, {}), this);
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
	RaiseChangedLevel(verbose, static_cast<short>(level));	
}

void GameCommands::ToggleMusic(const bool beVerbose) const
{
	if (logCommands) { Logger::Get()->LogThis("GameCommand: ToggleMusic", beVerbose); }

	if (!Mix_PlayingMusic() && !Mix_PausedMusic())
	{
		// We always plat level 4's music if no music is playing
		AudioManager::Get()->Play(AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo("LevelMusic4")));
	}
		
	Mix_PausedMusic() == 1 ? Mix_ResumeMusic() : Mix_PauseMusic();	
}

void GameCommands::Quit(const bool beVerbose) const
{
	if (logCommands) { Logger::Get()->LogThis("GameCommand: Quitting", beVerbose); }

	GameDataManager::Get()->GameWorldData.IsGameDone = true;
}

void GameCommands::InvalidMove(const bool beVerbose) const
{
	if (logCommands) { Logger::Get()->LogThis("GameCommand: Invalid move!", beVerbose); }
}

void GameCommands::FetchedPickup(const bool beVerbose) const
{
	if (logCommands) { Logger::Get()->LogThis("GameCommand: FetchedPickup", beVerbose); }

	AudioManager::Get()->Play(ResourceManager::Get()->GetAssetInfo(SettingsManager::String("audio", "fetched_pickup")));
}

void GameCommands::StartNetworkLevel()
{
	// This only works on the Game server
	if(GameData::Get()->IsMultiPlayerGame()) { return; }

	if (logCommands) { Logger::Get()->LogThis("GameCommand: StartNetworkLevel", verbose); }

	// Ask the LevelManager to prepare a level description and pass that to StartNetworkLevelEvent
	// and let that propagate to all players

	EventManager::Get()->RaiseEvent(EventFactory::Get()->CreateStartNetworkLevelEvent(1), this);
}

void GameCommands::PingGameServer(const unsigned long deltaMs) { NetworkManager::Get()->PingGameServer(deltaMs); }	

ListOfEvents GameCommands::HandleEvent(const std::shared_ptr<Event>& evt, const unsigned long deltaMs)
{	
	// Consider handling all game level events in LevelManager.cpp which then call into GameCommands.cpp
	return {};
}