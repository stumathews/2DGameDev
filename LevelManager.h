#pragma once

#include <memory>
#include "util/SettingsManager.h"
#include <objects/MultipleInheritableEnableSharedFromThis.h>
#include <iostream>
#include "game/gameWorld.h"
#include "common/Common.h"
#include "GameStructure.h"
#include "font/FontManager.h"
#include <events/EventManager.h>
#include <events/EventSubscriber.h>
#include "Pickup.h"
#include "audio/AudioManager.h"
#include "common/constants.h"
#include "events/AddGameObjectToCurrentSceneEvent.h"

#include "game/LevelGenerator.h"
#include "game/exceptions/game_exception.h"
#include "graphic/sdl_graphics_manager.h"
#include "resource/ResourceManager.h"
#include "scene/SceneManager.h"
#include "util/SettingsManager.h"
#include "objects/game_world_component.h"
#include <events/GameObjectEvent.h>
#include <events/Event.h>
#include "common/aliases.h"
#include "GameCommands.h"

class LevelManager : public gamelib::EventSubscriber, public inheritable_enable_shared_from_this<LevelManager>
{
public:
	std::string get_subscriber_name() override { return "level_manager";};
	void GetKeyboardInput();

	// Handle Level events
	std::vector<std::shared_ptr<gamelib::event>> handle_event(std::shared_ptr<gamelib::event> evt) override;
	
	// set basic game world defaults
	void InitGameWorldData() const;
	LevelManager(
		gamelib::EventManager& event_admin, 
		gamelib::ResourceManager& resource_admin, 
		gamelib::SettingsManager& settings_admin, 
		GameWorld& world,
		gamelib::SceneManager& scene_admin,
		gamelib::AudioManager& audio_admin, gamelib::logger& gameLogger);

	bool initialize();
	static size_t get_random_index(const int min, const int max);	
	gamelib::game_objects CreateLevel();
	
private:
	shared_ptr<gamelib::GameObject> CreatePlayer(vector<shared_ptr<gamelib::Room>> rooms, const int w, const int h) const;
	gamelib::game_objects CreatePickups(const vector<shared_ptr<gamelib::Room>>& rooms, const int w, const int h);
	gamelib::EventManager& event_admin;
	gamelib::ResourceManager& resource_admin;
	gamelib::SettingsManager& settings_admin;
	GameWorld& world;
	gamelib::SceneManager& scene_admin;
	gamelib::AudioManager& audio_admin;
	std::shared_ptr<GameCommands> gameCommands;
	gamelib::logger& gameLogger;

    void RemoveGameObject(GameWorld& gameWorld, gamelib::GameObject& gameObject);

};


