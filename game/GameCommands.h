#pragma once
#include "file/SettingsManager.h"
#include "events/EventManager.h"
#include <events/EventSubscriber.h>
#include <memory>
#include "audio/AudioManager.h"
#include <resource/ResourceManager.h>
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

	std::string GetSubscriberName() override;

	void Fire(bool beVerbose);
	void MoveUp(const bool beVerbose, const gamelib::ControllerMoveEvent::KeyState keyState);
	void MoveDown(const bool beVerbose, const gamelib::ControllerMoveEvent::KeyState keyState);
	void MoveLeft(const bool beVerbose, const gamelib::ControllerMoveEvent::KeyState keyState);
	void MoveRight(const bool beVerbose, const gamelib::ControllerMoveEvent::KeyState keyState);
	void Move(const gamelib::Direction direction, gamelib::ControllerMoveEvent::KeyState keyState);
	void PlaySoundEffect(const std::shared_ptr<gamelib::AudioAsset>& effect) const;
	void RaiseChangedLevel(bool beVerbose, short newLevel);
	void ReloadSettings(bool beVerbose);
	void LoadNewLevel(int level);
	void ToggleMusic(bool beVerbose) const;
	void Quit(bool beVerbose) const;
	void InvalidMove(bool beVerbose = false) const;
	void FetchedPickup(bool beVerbose = false) const;
	void StartNetworkLevel();
	static void PingGameServer();
private:
	bool verbose;
	bool logCommands;

	// Inherited via EventSubscriber
	gamelib::ListOfEvents HandleEvent(const std::shared_ptr<gamelib::Event>& evt, const unsigned long deltaMs) override;
};


