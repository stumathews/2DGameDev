#pragma once
#include "events/event_subscriber.h"
#include "objects/Player.h"

class game_structure final : public event_subscriber
{
public:
	game_structure();
	
	static void init_game_world_data();
	static bool initialize_sdl(int screen_width, int screen_height);
	
	std::shared_ptr<player> create_player() const;
	void setup_player() const;
	static bool initialize();

	void game_loop();

	void update();
	void world_update();
	void player_update();
	void get_input();
	
	static void draw(float);

	/**
	 * Parses game resources, generates level and adds player to scene
	 */
	bool load_content() const;
	static long get_tick_now();

	/***
	 Ask the event manager to process events
	 */
	static void spare_time(long);

	static void unload();	 // releases resources

	std::vector<std::shared_ptr<event>> handle_event(std::shared_ptr<event> the_event) override;
	std::string get_subscriber_name() override;
};
