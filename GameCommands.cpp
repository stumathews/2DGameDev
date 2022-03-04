#include "GameCommands.h"
#include "common/common.h"
#include <memory>
#include "events/PositionChangeEvent.h"
#include "events/SceneChangedEvent.h"

#include <iostream>
using namespace gamelib;
using namespace std;


GameCommands::GameCommands(std::shared_ptr<settings_manager> settings, 
						   std::shared_ptr<event_manager> events,
						   std::shared_ptr<gamelib::audio_manager> audio,
						   std::shared_ptr<gamelib::resource_manager> resources,
						   std::shared_ptr<GameWorld> gameWorld) 
	: _settings(settings), _events(events), _be_verbose(false), _audio(audio), _resources(resources), _gameWorld(gameWorld)
{
}

GameCommands::GameCommands() : _be_verbose(false)
{
}

void GameCommands::Fire(bool be_verbose)
{
	run_and_log("Space bar pressed!", be_verbose, [&]()
	{
		// Raise Fire event
		_events->raise_event(std::make_shared<gamelib::event>(gamelib::event_type::Fire), shared_from_this());
		return true;
	}, true, true, _settings);
}

void GameCommands::MoveUp(bool be_verbose)
{
	run_and_log("Player pressed up!", be_verbose, [&]()
	{
		// Raise Position changed event - Up
		_events->raise_event(std::make_unique<gamelib::position_change_event>(gamelib::Direction::Up), shared_from_this());
		return true;
	}, true, true, _settings);
}

void GameCommands::MoveDown(bool be_verbose)
{
	run_and_log("Player pressed down!", be_verbose, [&]()
	{
		// Raise Position changed event - Down
		_events->raise_event(std::make_unique<gamelib::position_change_event>(gamelib::Direction::Down), shared_from_this());
		return true;
	}, true, true, _settings);
}

void GameCommands::MoveLeft(bool be_verbose)
{
	run_and_log("Player pressed left!", be_verbose, [&]()
	{
		// Raise Position changed event - Left
		_events->raise_event(std::make_unique<gamelib::position_change_event>(gamelib::Direction::Left), shared_from_this());
		return true;
	}, true, true, _settings);
}

void GameCommands::MoveRight(bool be_verbose)
{
	run_and_log("Player pressed right!", be_verbose, [&]()
	{
		// Raise Position changed event - Rigt
		_events->raise_event(std::make_unique<gamelib::position_change_event>(gamelib::Direction::Right), shared_from_this());
		return true;
	}, true, true, _settings);
}

void GameCommands::ChangeLevel(bool be_verbose, short newLevel)
{
	string message = "Change to level " + newLevel;

	run_and_log(message, be_verbose, [&]()
	{
		_events->raise_event(std::make_unique<gamelib::scene_changed_event>(newLevel), shared_from_this());
		return true;
	}, true, true, _settings);
}

void GameCommands::ReloadSettings(bool be_verbose)
{
	_settings->reload();
	_events->raise_event(make_shared<gamelib::event>(gamelib::event_type::SettingsReloaded), shared_from_this());
	log_message("Settings reloaded", be_verbose, false);
}

void GameCommands::GenerateNewLevel(bool be_verbose)
{
	_events->raise_event(make_shared<gamelib::event>(gamelib::event_type::GenerateNewLevel), shared_from_this());
	log_message("Generating new level", be_verbose, false);
}

void GameCommands::ToggleMusic(bool be_verbose)
{	
	if (!Mix_PlayingMusic())
	{
		_audio->play_music(gamelib::audio_manager::to_resource(_resources->get("MainTheme.wav"))->as_music());
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
		_gameWorld->is_game_done = 1;
		return true;
	}, true, true, _settings);
}

void GameCommands::InvalidMove(bool be_verbose)
{
	_audio->play_sound(audio_manager::to_resource(_resources->get(_settings->get_string("audio", "invalid_move")))->as_fx());
}

void GameCommands::FetchedPickup(bool be_verbose)
{
	_audio->play_sound(audio_manager::to_resource(_resources->get(_settings->get_string("audio", "fetched_pickup")))->as_fx());
}

std::vector<std::shared_ptr<event>> GameCommands::handle_event(std::shared_ptr<event> evt)
{
	return std::vector<std::shared_ptr<event>>();
}
