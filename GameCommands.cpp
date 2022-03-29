#include "GameCommands.h"
#include "common/common.h"
#include <memory>
#include "events/PositionChangeEvent.h"
#include "events/SceneChangedEvent.h"
#include "scene/SceneManager.h"
#include <iostream>
using namespace gamelib;
using namespace std;

/// <summary>
/// GameCommands is a common place where game events are raised in the game
/// </summary>

GameCommands::GameCommands() : _be_verbose(false) { }

std::string GameCommands::GetSubscriberName() 
{
	return "GameCommands";
}

/// <summary>
/// Fire!
/// </summary>
/// <param name="be_verbose"></param>
void GameCommands::Fire(bool be_verbose)
{
	LogThis("Space bar pressed!", be_verbose, [&]()
	{
		// Raise Fire event
		EventManager::Get()->RaiseEvent(std::make_shared<gamelib::Event>(gamelib::EventType::Fire), this);
		return true;
	}, true, true);
}

/// <summary>
/// Move Up
/// </summary>
/// <param name="be_verbose"></param>
void GameCommands::MoveUp(bool be_verbose)
{
	LogThis("Player pressed up!", be_verbose, [&]()
	{
		// Raise Position changed event - Up
		EventManager::Get()->RaiseEvent(std::make_unique<gamelib::PositionChangeEvent>(gamelib::Direction::Up), this);
		return true;
	}, true, true);
}

/// <summary>
/// Move down
/// </summary>
/// <param name="be_verbose"></param>
void GameCommands::MoveDown(bool be_verbose)
{
	LogThis("Player pressed down!", be_verbose, [&]()
	{
		// Raise Position changed event - Down
		EventManager::Get()->RaiseEvent(std::make_unique<gamelib::PositionChangeEvent>(gamelib::Direction::Down), this);
		return true;
	}, true, true);
}

/// <summary>
/// Move left
/// </summary>
/// <param name="be_verbose"></param>
void GameCommands::MoveLeft(bool be_verbose)
{
	LogThis("Player pressed left!", be_verbose, [&]()
	{
		// Raise Position changed event - Left
		EventManager::Get()->RaiseEvent(std::make_unique<gamelib::PositionChangeEvent>(gamelib::Direction::Left), this);
		return true;
	}, true, true);
}

/// <summary>
/// Move right
/// </summary>
/// <param name="be_verbose"></param>
void GameCommands::MoveRight(bool be_verbose)
{
	LogThis("Player pressed right!", be_verbose, [&]()
	{
		// Raise Position changed event - Rigt
		EventManager::Get()->RaiseEvent(std::make_unique<gamelib::PositionChangeEvent>(gamelib::Direction::Right), this);
		return true;
	}, true, true);
}

void GameCommands::ChangeLevel(bool be_verbose, short newLevel)
{
	string message = "Change to level " + newLevel;

	LogThis(message, be_verbose, [&]()
	{
		EventManager::Get()->RaiseEvent(std::make_unique<gamelib::SceneChangedEvent>(newLevel), this);
		return true;
	}, true, true);
}

/// <summary>
/// Reload Settings
/// </summary>
/// <param name="be_verbose"></param>
void GameCommands::ReloadSettings(bool be_verbose)
{
	SettingsManager::Get()->Reload();
	EventManager::Get()->RaiseEvent(make_shared<gamelib::Event>(gamelib::EventType::SettingsReloaded), this);
	LogMessage("Settings reloaded", be_verbose, false);
}

/// <summary>
/// Generate new level
/// </summary>
/// <param name="be_verbose"></param>
void GameCommands::GenerateNewLevel(bool be_verbose)
{
	EventManager::Get()->RaiseEvent(make_shared<gamelib::Event>(gamelib::EventType::GenerateNewLevel), this);
	LogMessage("Generating new level", be_verbose, false);
}

/// <summary>
/// Toggled playing music
/// </summary>
/// <param name="be_verbose"></param>
void GameCommands::ToggleMusic(bool be_verbose)
{	
	if (!Mix_PlayingMusic())
	{
		AudioManager::Get()->PlayMusic(gamelib::AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo("MainTheme.wav"))->AsMusic());
	}
	else
	{
		if (Mix_PausedMusic() == 1)
			Mix_ResumeMusic();
		else
			Mix_PauseMusic();
	}
}

/// <summary>
/// Quite game
/// </summary>
/// <param name="be_verbose"></param>
void GameCommands::Quit(bool be_verbose)
{	
	LogThis("Player pressed quit!", be_verbose, [&]()
	{		
		SceneManager::Get()->GetGameWorld().IsGameDone = 1;
		return true;
	}, true, true);
}

/// <summary>
/// Invalid Move command
/// </summary>
/// <param name="be_verbose"></param>
void GameCommands::InvalidMove(bool be_verbose)
{
	Logger::Get()->LogThis("GameCommand: Invalid move", be_verbose);
	AudioManager::Get()->PlaySound(AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo(SettingsManager::Get()->GetString("audio", "invalid_move")))->AsSoundEffect());
}

/// <summary>
/// Pickup fetch command
/// </summary>
/// <param name="be_verbose"></param>
void GameCommands::FetchedPickup(bool be_verbose)
{
	AudioManager::Get()->PlaySound(AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo(SettingsManager::Get()->GetString("audio", "fetched_pickup")))->AsSoundEffect());
}

/// <summary>
/// Handle game commands
/// </summary>
/// <param name="evt"></param>
/// <returns></returns>
std::vector<std::shared_ptr<Event>> GameCommands::HandleEvent(std::shared_ptr<Event> evt)
{
	// Dont currently handle any events yet
	return std::vector<std::shared_ptr<Event>>();
}
