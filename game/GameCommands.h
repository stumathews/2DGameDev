#pragma once
#include "file/SettingsManager.h"
#include "events/EventManager.h"
#include <events/EventSubscriber.h>
#include <memory>
#include "audio/AudioManager.h"
#include <resource/ResourceManager.h>
#include "GameWorld.h"
#include <events/Event.h>

#include "events/ControllerMoveEvent.h"

class GameCommands final : public gamelib::EventSubscriber, public std::enable_shared_from_this<GameCommands>
{
public:	
	GameCommands();
	~GameCommands() override = default;
	GameCommands(const GameCommands& copy) = delete;
	GameCommands(const GameCommands&& copy) = delete;
	GameCommands& operator=(const GameCommands& other) = delete;
	GameCommands& operator=(const GameCommands&& other) = delete;

	std::string GetSubscriberName() override { return "GameCommands"; }

	void Fire(bool verbose);
	void MoveUp(const bool verbose, const gamelib::ControllerMoveEvent::KeyState keyState);
	void MoveDown(const bool verbose, const gamelib::ControllerMoveEvent::KeyState keyState);
	void MoveLeft(const bool verbose, const gamelib::ControllerMoveEvent::KeyState keyState);
	void MoveRight(const bool verbose, const gamelib::ControllerMoveEvent::KeyState keyState);
	void Move(const gamelib::Direction direction, gamelib::ControllerMoveEvent::KeyState keyState);
	void PlaySoundEffect(const std::shared_ptr<gamelib::AudioAsset>& effect) const;
	void RaiseChangedLevel(bool verbose, short newLevel);
	void ReloadSettings(bool verbose);
	void LoadNewLevel(int level);
	void ToggleMusic(bool verbose) const;
	void Quit(bool verbose) const;
	void InvalidMove(bool verbose = false) const;
	void FetchedPickup(bool verbose = false) const;
	void StartNetworkLevel();
	static void PingGameServer();
private:
	bool verbose;
	bool logCommands;

	// Inherited via EventSubscriber
	virtual std::vector<std::shared_ptr<gamelib::Event>> HandleEvent(std::shared_ptr<gamelib::Event> evt, unsigned long deltaMs) override;
};


