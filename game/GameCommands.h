#pragma once
#include "util/SettingsManager.h"
#include "events/EventManager.h"
#include <objects/MultipleInheritableEnableSharedFromThis.h>
#include <events/EventSubscriber.h>
#include <memory>
#include "audio/AudioManager.h"
#include <resource/ResourceManager.h>
#include "GameWorld.h"
#include <events/Event.h>

class GameCommands :  public gamelib::EventSubscriber, public inheritable_enable_shared_from_this<GameCommands>
{
public:	
	GameCommands();
	GameCommands(const GameCommands& copy) = delete;

	std::string GetSubscriberName() override { return "GameCommands"; }

	void Fire(bool verbose);
	void MoveUp(bool verbose);
	void MoveDown(bool verbose);
	void MoveLeft(bool verbose);
	void MoveRight(bool verbose);
	void PlaySoundEffect(Mix_Chunk* effect) const;
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
	bool _verbose;
	bool logCommands;

	// Inherited via EventSubscriber
	virtual std::vector<std::shared_ptr<gamelib::Event>> HandleEvent(std::shared_ptr<gamelib::Event> evt, unsigned long deltaMs) override;
};


