#include "GameCommands.h"
#include "common/common.h"
#include <memory>
#include "events/PositionChangeEvent.h"
#include "events/SceneChangedEvent.h"

#include <iostream>
using namespace gamelib;
using namespace std;

/// <summary>
/// GameCommands is a common place where game events are raised in the game
/// </summary>
/// <param name="settings"></param>
/// <param name="events"></param>
/// <param name="audio"></param>
/// <param name="resources"></param>
/// <param name="gameWorld"></param>
/// <param name="gameLogger"></param>
GameCommands::GameCommands(SettingsManager& settings, 
						   EventManager& events,
						   AudioManager& audio,
						   ResourceManager& resources,
						   GameWorld& gameWorld, 
						   Logger& gameLogger) 
	: _settings(settings), _events(events), _be_verbose(false), _audioManager(audio), _resources(resources), _gameWorld(gameWorld), _gameLogger(gameLogger) { }

/// <summary>
/// Fire!
/// </summary>
/// <param name="be_verbose"></param>
void GameCommands::Fire(bool be_verbose)
{
	LogThis("Space bar pressed!", be_verbose, [&]()
	{
		// Raise Fire event
		_events.RaiseEvent(std::make_shared<gamelib::Event>(gamelib::EventType::Fire), this);
		return true;
	}, _settings, true, true);
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
		_events.RaiseEvent(std::make_unique<gamelib::position_change_event>(gamelib::Direction::Up), this);
		return true;
	}, _settings, true, true);
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
		_events.RaiseEvent(std::make_unique<gamelib::position_change_event>(gamelib::Direction::Down), this);
		return true;
	}, _settings, true, true);
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
		_events.RaiseEvent(std::make_unique<gamelib::position_change_event>(gamelib::Direction::Left), this);
		return true;
	}, _settings, true, true);
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
		_events.RaiseEvent(std::make_unique<gamelib::position_change_event>(gamelib::Direction::Right), this);
		return true;
	}, _settings, true, true);
}

void GameCommands::ChangeLevel(bool be_verbose, short newLevel)
{
	string message = "Change to level " + newLevel;

	LogThis(message, be_verbose, [&]()
	{
		_events.RaiseEvent(std::make_unique<gamelib::SceneChangedEvent>(newLevel), this);
		return true;
	}, _settings, true, true);
}

/// <summary>
/// Reload Settings
/// </summary>
/// <param name="be_verbose"></param>
void GameCommands::ReloadSettings(bool be_verbose)
{
	_settings.reload();
	_events.RaiseEvent(make_shared<gamelib::Event>(gamelib::EventType::SettingsReloaded), this);
	LogMessage("Settings reloaded", _gameLogger, be_verbose, false);
}

/// <summary>
/// Generate new level
/// </summary>
/// <param name="be_verbose"></param>
void GameCommands::GenerateNewLevel(bool be_verbose)
{
	_events.RaiseEvent(make_shared<gamelib::Event>(gamelib::EventType::GenerateNewLevel), this);
	LogMessage("Generating new level", _gameLogger, be_verbose, false);
}

/// <summary>
/// Toggled playing music
/// </summary>
/// <param name="be_verbose"></param>
void GameCommands::ToggleMusic(bool be_verbose)
{	
	if (!Mix_PlayingMusic())
	{
		_audioManager.PlayMusic(gamelib::AudioManager::ToAudioAsset(_resources.GetAssetInfo("MainTheme.wav"))->AsMusic());
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
		_gameWorld.IsGameDone = 1;
		return true;
	}, _settings, true, true);
}

/// <summary>
/// Invalid Move command
/// </summary>
/// <param name="be_verbose"></param>
void GameCommands::InvalidMove(bool be_verbose)
{
	_gameLogger.LogThis("GameCommand: Invalid move", be_verbose);
	_audioManager.PlaySound(AudioManager::ToAudioAsset(_resources.GetAssetInfo(_settings.get_string("audio", "invalid_move")))->AsSoundEffect());
}

/// <summary>
/// Pickup fetch command
/// </summary>
/// <param name="be_verbose"></param>
void GameCommands::FetchedPickup(bool be_verbose)
{
	_audioManager.PlaySound(AudioManager::ToAudioAsset(_resources.GetAssetInfo(_settings.get_string("audio", "fetched_pickup")))->AsSoundEffect());
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
