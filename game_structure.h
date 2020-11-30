#pragma once
#include "Player.h"

class game_structure final : public event_subscriber
{
public:
	game_structure();
	static void init_game_world_data();

	static bool initialize(int screen_width, int screen_height);
	static bool init3d_render_manager();
	shared_ptr<player> create_player() const;
	void setup_player() const;
	static bool initialize();
	void game_loop();
	bool load_content() const;
	static long get_tick_now();
	void world_update();
	void update();
	static void draw(float);
	static void spare_time(long);
	static bool initialize_sdl(int screen_width, int screen_height);
	static void unload();
	void get_input();
	
	void player_update();

	vector<shared_ptr<event>> handle_event(std::shared_ptr<event> evt) override;
	string get_subscriber_name() override;
};
