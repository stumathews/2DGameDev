#pragma once
#include "util/SettingsManager.h"
#include "events/EventManager.h"
#include <objects/MultipleInheritableEnableSharedFromThis.h>
#include <events/EventSubscriber.h>
#include <memory>
#include "audio/AudioManager.h"
#include <resource/ResourceManager.h>
#include "GameWorld.h"

class GameCommands :  public gamelib::EventSubscriber, public inheritable_enable_shared_from_this<GameCommands>
{
public:	
	GameCommands();
	GameCommands(const GameCommands& copy) = delete;

	std::string GetSubscriberName() override { return "GameCommands"; }

	void Fire(bool _verbose);
	void MoveUp(bool _verbose);
	void MoveDown(bool _verbose);
	void MoveLeft(bool _verbose);
	void MoveRight(bool _verbose);
	void PlaySoundEffect(Mix_Chunk* effect);
	void RaiseChangedLevel(bool _verbose, short newLevel);
	void ReloadSettings(bool _verbose);
	void GenerateNewLevel(bool _verbose);
	void LoadNewLevel(int level);
	void ToggleMusic(bool _verbose);
	void Quit(bool _verbose);
	void InvalidMove(bool _verbose = false);
	void FetchedPickup(bool _verbose = false);
	void StartNetworkLevel();
	void PingGameServer();
private:
	bool _verbose;
	bool logCommands;

	// Inherited via EventSubscriber
	virtual std::vector<std::shared_ptr<gamelib::Event>> HandleEvent(std::shared_ptr<gamelib::Event> evt) override;
};


