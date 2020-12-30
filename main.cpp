#include <SDL.h>
#include <Windows.h>
#include <iostream>
#include "game/game_world.h"
#include "common/Common.h"
#include "game_structure.h"
#include "font/font_manager.h"
#include <events/event_manager.h>
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

using namespace std;
using namespace gamelib;

class level_manager : event_subscriber
{
public:
	std::string get_subscriber_name() override { return "level_manager";};
	std::vector<std::shared_ptr<event>> handle_event(std::shared_ptr<event> evt) override;
	void init_game_world_data() const;
	level_manager(std::shared_ptr<event_manager> event_admin,
	                                             std::shared_ptr<resource_manager> resource_admin,
	                                             std::shared_ptr<settings_manager> settings_admin, shared_ptr<game_world> world, shared_ptr<scene_manager> scene_admin);
	bool initialize();

	shared_ptr<player> create_player(shared_ptr<settings_manager> settings_admin,
	                                 shared_ptr<resource_manager> resource_admin) const;
	shared_ptr<game_object> setup_player(vector<shared_ptr<square>> rooms) const;
	vector<shared_ptr<game_object>> load_content();
	std::shared_ptr<event_manager> event_admin;
	std::shared_ptr<resource_manager> resource_admin;
	std::shared_ptr<settings_manager> settings_admin;
	shared_ptr<game_world> world;
	shared_ptr<scene_manager> scene_admin;
};

events level_manager::handle_event(std::shared_ptr<event> evt)
{
	events secondary_events;
	if(evt->get_type() == event_type::GenerateNewLevel)
	{
		load_content();
	}
	return secondary_events;
}

void level_manager::init_game_world_data() const
{
	world->is_game_done = false;
	world->is_network_game = false;
	world->can_render = true;
}

level_manager::level_manager(std::shared_ptr<event_manager> event_admin,
	std::shared_ptr<resource_manager> resource_admin, std::shared_ptr<settings_manager> settings_admin, shared_ptr<game_world> world, shared_ptr<scene_manager> scene_admin)
	: event_admin(std::move(event_admin)), resource_admin(std::move(resource_admin)), settings_admin(std::move(settings_admin)), world(std::move(world)), scene_admin(std::move(scene_admin))
{
	
}

bool level_manager::initialize()
{
	init_game_world_data();
	
	event_admin->subscribe_to_event(event_type::GenerateNewLevel, this);
	return true;
}

shared_ptr<player> level_manager::create_player(shared_ptr<settings_manager> settings_admin, shared_ptr<resource_manager> resource_admin) const
{
	return make_shared<player>(player(settings_admin->get_int("player","player_init_pos_x"), settings_admin->get_int("player", "player_init_pos_y"), settings_admin->get_int("global", "square_width") / 2, resource_admin, settings_admin));
}

shared_ptr<game_object> level_manager::setup_player(vector<shared_ptr<square>> rooms) const
{
	const auto the_player = create_player(settings_admin, resource_admin);
	const auto room_component = make_shared<component>("rooms");
	
	the_player->load_settings(settings_admin);

	the_player->subscribe_to_event(event_type::PositionChangeEventType, event_admin);			
	the_player->subscribe_to_event(event_type::SettingsReloaded, event_admin);


	the_player->set_tag(constants::playerTag);
	the_player->add_component(room_component);

	the_player->raise_event(std::make_shared<add_game_object_to_current_scene_event>(the_player, 100), event_admin);

	const auto min = 0;
	const auto random_room_index = rand() % (rooms.size()-min+1) + min;
	the_player->set_room_within(random_room_index);

	return the_player;
}

vector<shared_ptr<game_object>> level_manager::load_content()
{	
	resource_admin->read_resources();

	vector<shared_ptr<game_object>> game_objects;
	
	
	// Generate the level's rooms
	auto rooms = level_generator::generate_level(resource_admin, settings_admin);
	for (const auto& room: rooms)
	{			
		// room's will want to know when the player moved for collision detection etc
		room->subscribe_to_event(event_type::PlayerMovedEventType, event_admin);
		
		// Add each room to the scene
		room->raise_event(std::make_shared<add_game_object_to_current_scene_event>(std::dynamic_pointer_cast<square>(room)), event_admin);
					
		room->load_settings(settings_admin);
		room->subscribe_to_event(event_type::SettingsReloaded, event_admin);

		game_objects.push_back( dynamic_pointer_cast<game_object>(room));
	}
	
	
	// Create the player
	const auto player = setup_player(rooms);
	

	// Add player to game world
	game_objects.push_back(player);

	// Setup game world
	world->game_objects = game_objects;	
	const auto game_world_component = make_shared<class game_world_component>(world);

	// Add the game world as a component on the player
	player->add_component(game_world_component);
	
	return game_objects;
}

int main(int argc, char *args[])
{
	try
	{
		const auto world = make_shared<game_world>();		
		const auto settings_admin = make_shared<settings_manager>();
		const auto event_admin = make_shared<event_manager>(settings_admin);
		const auto graphics_admin = make_shared<sdl_graphics_manager>(event_admin);
		const auto audio_admin = make_shared<audio_manager>(); // makes audio resources
		const auto font_admin = make_shared<font_manager>(); // makes font resources
		const auto resource_admin = make_shared<resource_manager>(settings_admin, graphics_admin, font_admin, audio_admin); 
		const auto scene_admin = make_shared<scene_manager>(event_admin, settings_admin, resource_admin);
		const auto structure =  make_shared<game_structure>(event_admin, resource_admin, settings_admin, world, scene_admin, graphics_admin);
		const auto level_admin = make_shared<level_manager>(event_admin, resource_admin, settings_admin, world, scene_admin);
		
		
		

		// Game Initialization
		
		const auto is_initialized = succeeded(structure->initialize(), "Initialize");
		const auto is_loaded = succeeded(level_admin->initialize(), "loading content");

		
		level_admin->load_content();

		if(!is_initialized || !is_loaded)
			return -1;

		// Game Loop
		
		const auto loop_result = run_and_log("Beginning game loop...", settings_admin->get_bool("global","verbose"), [&]()
		{
			return structure->game_loop();
		}, true, true, settings_admin);

		// Unload Game
		
		const auto unload_result = run_and_log("Unloading game...", settings_admin->get_bool("global","verbose"), [&]()
		{
			return structure->unload();
		}, true, true, settings_admin);
		
		return succeeded(loop_result, "game loop failed") && 
			   succeeded(unload_result, "content unload failed");
	
	}
	catch(game_exception& game_exception)
	{
		log_message("Game Exception occurred from " + string(game_exception.get_source_subsystem()) + string(": ") + string(game_exception.what()));
	}
	return 0;
}
