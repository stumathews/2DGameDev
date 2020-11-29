#pragma once
#include "Player.h"

class game_structure final : public event_subscriber
{
public:
	game_structure();
	std::shared_ptr<game_world_data> g_pGameWorldData;
	void init_game_world_data() const;

	static bool load_media();
	static bool initialize(int screen_width, int screen_height);
	static void init3d_render_manager();
	static shared_ptr<player> create_player();
	static void add_player_to_scene();
	static bool initialize();
	static void game_loop();
	static bool load_content();
	static long get_tick_now();
	void update_active_elements();
	void world_update();
	void update();
	static void draw(float);
	static void spare_time(long);
	static bool init_sdl(int screenWidth, int screenHeight);
	static void unload();
	void get_input();
	
	void player_update();

	void update_state();
	void update_logic_based_elements();
	vector<shared_ptr<event>> process_event(std::shared_ptr<event> evt) override;
	string get_subscriber_name() override;
};
