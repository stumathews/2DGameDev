#pragma once
#include "Player.h"

class game_structure final : public event_subscriber
{
public:
	game_structure();
	void init_game_world_data() const;

	static bool load_media();
	bool initialize(int screen_width, int screen_height);
	static void init3d_render_manager();
	shared_ptr<player> create_player() const;
	void setup_player() const;
	static bool initialize();
	void game_loop();
	bool load_content() const;
	static long get_tick_now();
	void update_active_elements();
	void world_update();
	void update();
	static void draw(float);
	static void spare_time(long);
	static bool initialize_sdl(int screenWidth, int screenHeight);
	static void unload();
	void get_input();
	
	void player_update();

	vector<shared_ptr<event>> process_event(std::shared_ptr<event> evt) override;
	string get_subscriber_name() override;
};
