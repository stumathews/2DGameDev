#pragma once
#include "util/settings_manager.h"
#include "events/event_manager.h"
#include <objects/MultipleInheritableEnableSharedFromThis.h>
#include <events/EventSubscriber.h>
#include <memory>
#include "audio/AudioManager.h"
#include <resource/resource_manager.h>
#include "game/GameWorld.h"

class GameCommands :  public gamelib::EventSubscriber, public inheritable_enable_shared_from_this<GameCommands>
{
public:
	GameCommands(const GameCommands& copy) = delete;
	GameCommands& operator=(GameCommands& other) = delete;
	GameCommands(std::shared_ptr<gamelib::settings_manager> settings, 
				 std::shared_ptr<gamelib::event_manager> events,
				 std::shared_ptr<gamelib::audio_manager> audio,
				 std::shared_ptr<gamelib::resource_manager> resources,
				 std::shared_ptr<GameWorld> gameWorld);
	GameCommands();

	std::string get_subscriber_name() override { return "GameCommands";};

	void Fire(bool be_verbose);
	void MoveUp(bool be_verbose);
	void MoveDown(bool be_verbose);
	void MoveLeft(bool be_verbose);
	void MoveRight(bool be_verbose);
	void ChangeLevel(bool be_verbose, short newLevel);
	void ReloadSettings(bool be_verbose);
	void GenerateNewLevel(bool be_verbose);
	void ToggleMusic(bool be_verbose);
	void Quit(bool be_verbose);
	void InvalidMove(bool be_verbose = false);
	void FetchedPickup(bool be_verbose = false);
private:
	std::shared_ptr<gamelib::settings_manager> _settings;
	std::shared_ptr<gamelib::event_manager> _events;
	std::shared_ptr<gamelib::audio_manager> _audio;
	std::shared_ptr<gamelib::resource_manager> _resources;
	std::shared_ptr<GameWorld> _gameWorld;
	bool _be_verbose;

	// Inherited via EventSubscriber
	virtual std::vector<std::shared_ptr<gamelib::event>> handle_event(std::shared_ptr<gamelib::event> evt) override;
};


