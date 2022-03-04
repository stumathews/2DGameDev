#pragma once

#include <memory>
#include "util/settings_manager.h"
#include <objects/MultipleInheritableEnableSharedFromThis.h>
#include <iostream>
#include "game/gameWorld.h"
#include "common/Common.h"
#include "game_structure.h"
#include "font/font_manager.h"
#include <events/event_manager.h>
#include <events/EventSubscriber.h>
#include "Pickup.h"
#include "audio/AudioManager.h"
#include "common/constants.h"
#include "events/AddGameObjectToCurrentSceneEvent.h"

#include "game/LevelGenerator.h"
#include "game/exceptions/game_exception.h"
#include "graphic/sdl_graphics_manager.h"
#include "resource/resource_manager.h"
#include "scene/scene_manager.h"
#include "util/settings_manager.h"
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
	void init_game_world_data() const;
	LevelManager(std::shared_ptr<gamelib::event_manager> event_admin,
	                                             shared_ptr<gamelib::resource_manager> resource_admin,
	                                             shared_ptr<gamelib::settings_manager> settings_admin, 
												 shared_ptr<GameWorld> world,
												 shared_ptr<gamelib::scene_manager> scene_admin, shared_ptr<gamelib::audio_manager> audio_admin);
	bool initialize();
	static size_t get_random_index(const int min, const int max);	
	gamelib::game_objects CreateLevel();
	
private:
	shared_ptr<gamelib::GameObject> CreatePlayer(vector<shared_ptr<gamelib::Room>> rooms, const int w, const int h) const;
	gamelib::game_objects CreatePickups(const vector<shared_ptr<gamelib::Room>>& rooms, const int w, const int h);
	std::shared_ptr<gamelib::event_manager> event_admin;
	std::shared_ptr<gamelib::resource_manager> resource_admin;
	std::shared_ptr<gamelib::settings_manager> settings_admin;
	shared_ptr<GameWorld> world;
	shared_ptr<gamelib::scene_manager> scene_admin;
	shared_ptr<gamelib::audio_manager> audio_admin;
	shared_ptr<GameCommands> gameCommands;

};


