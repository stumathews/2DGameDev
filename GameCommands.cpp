#include "GameCommands.h"
#include "common/common.h"
#include <memory>
#include "events/PositionChangeEvent.h"
#include "events/SceneChangedEvent.h"

#include <iostream>
using namespace gamelib;
using namespace std;


GameCommands::GameCommands(SettingsManager& settings, 
						   EventManager& events,
						   AudioManager& audio,
						   ResourceManager& resources,
						   GameWorld& gameWorld, 
						   logger& gameLogger) 
	: _settings(settings), _events(events), _be_verbose(false), _audio(audio), _resources(resources), _gameWorld(gameWorld), _gameLogger(gameLogger)
{
}

void GameCommands::Fire(bool be_verbose)
{
	run_and_log("Space bar pressed!", be_verbose, [&]()
	{
		// Raise Fire event
		_events.raise_event(std::make_shared<gamelib::event>(gamelib::event_type::Fire), this);
		return true;
	}, _settings, true, true);
}

void GameCommands::MoveUp(bool be_verbose)
{
	run_and_log("Player pressed up!", be_verbose, [&]()
	{
		// Raise Position changed event - Up
		_events.raise_event(std::make_unique<gamelib::position_change_event>(gamelib::Direction::Up), this);
		return true;
	}, _settings, true, true);
}

void GameCommands::MoveDown(bool be_verbose)
{
	run_and_log("Player pressed down!", be_verbose, [&]()
	{
		// Raise Position changed event - Down
		_events.raise_event(std::make_unique<gamelib::position_change_event>(gamelib::Direction::Down), this);
		return true;
	}, _settings, true, true);
}

void GameCommands::MoveLeft(bool be_verbose)
{
	run_and_log("Player pressed left!", be_verbose, [&]()
	{
		// Raise Position changed event - Left
		_events.raise_event(std::make_unique<gamelib::position_change_event>(gamelib::Direction::Left), this);
		return true;
	}, _settings, true, true);
}

void GameCommands::MoveRight(bool be_verbose)
{
	run_and_log("Player pressed right!", be_verbose, [&]()
	{
		// Raise Position changed event - Rigt
		_events.raise_event(std::make_unique<gamelib::position_change_event>(gamelib::Direction::Right), this);
		return true;
	}, _settings, true, true);
}

void GameCommands::ChangeLevel(bool be_verbose, short newLevel)
{
	string message = "Change to level " + newLevel;

	run_and_log(message, be_verbose, [&]()
	{
		_events.raise_event(std::make_unique<gamelib::scene_changed_event>(newLevel), this);
		return true;
	}, _settings, true, true);
}

void GameCommands::ReloadSettings(bool be_verbose)
{
	_settings.reload();
	_events.raise_event(make_shared<gamelib::event>(gamelib::event_type::SettingsReloaded), this);
	log_message("Settings reloaded", _gameLogger, be_verbose, false);
}

void GameCommands::GenerateNewLevel(bool be_verbose)
{
	_events.raise_event(make_shared<gamelib::event>(gamelib::event_type::GenerateNewLevel), this);
	log_message("Generating new level", _gameLogger, be_verbose, false);
}

void GameCommands::ToggleMusic(bool be_verbose)
{	
	if (!Mix_PlayingMusic())
	{
		_audio.play_music(gamelib::AudioManager::to_resource(_resources.get("MainTheme.wav"))->as_music());
	}
	else
	{
		if (Mix_PausedMusic() == 1)
			Mix_ResumeMusic();
		else
			Mix_PauseMusic();
	}
}

void GameCommands::Quit(bool be_verbose)
{	
	run_and_log("Player pressed quit!", be_verbose, [&]()
	{
		_gameWorld.is_game_done = 1;
		return true;
	}, _settings, true, true);
}

void GameCommands::InvalidMove(bool be_verbose)
{
	_audio.play_sound(AudioManager::to_resource(_resources.get(_settings.get_string("audio", "invalid_move")))->as_fx());
}

void GameCommands::FetchedPickup(bool be_verbose)
{
	_audio.play_sound(AudioManager::to_resource(_resources.get(_settings.get_string("audio", "fetched_pickup")))->as_fx());
}

std::vector<std::shared_ptr<event>> GameCommands::handle_event(std::shared_ptr<event> evt)
{
	return std::vector<std::shared_ptr<event>>();
}
