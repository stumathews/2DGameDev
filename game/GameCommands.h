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

	std::string GetSubscriberName() override;;

	void Fire(bool verbose);
	void MoveUp(bool verbose);
	void MoveDown(bool verbose);
	void MoveLeft(bool verbose);
	void MoveRight(bool verbose);
	void PlaySoundEffect(Mix_Chunk* effect);
	void RaiseChangedLevel(bool verbose, short newLevel);
	void ReloadSettings(bool verbose);
	void GenerateNewLevel(bool verbose);
	void LoadNewLevel(int level);
	void ToggleMusic(bool verbose);
	void Quit(bool verbose);
	void InvalidMove(bool verbose = false);
	void FetchedPickup(bool verbose = false);
	void StartNetworkLevel();
	void PingGameServer();
private:
	bool verbose;
	bool logCommands;

	// Inherited via EventSubscriber
	virtual std::vector<std::shared_ptr<gamelib::Event>> HandleEvent(std::shared_ptr<gamelib::Event> evt) override;
};


