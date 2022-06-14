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

	void Fire(bool be_verbose);
	void MoveUp(bool be_verbose);
	void MoveDown(bool be_verbose);
	void MoveLeft(bool be_verbose);
	void MoveRight(bool be_verbose);
	void PlaySoundEffect(Mix_Chunk* effect);
	void ChangeLevel(bool be_verbose, short newLevel);
	void ReloadSettings(bool be_verbose);
	void GenerateNewLevel(bool be_verbose);
	void ToggleMusic(bool be_verbose);
	void Quit(bool be_verbose);
	void InvalidMove(bool be_verbose = false);
	void FetchedPickup(bool be_verbose = false);
private:
	bool _be_verbose;

	// Inherited via EventSubscriber
	virtual std::vector<std::shared_ptr<gamelib::Event>> HandleEvent(std::shared_ptr<gamelib::Event> evt) override;
};


