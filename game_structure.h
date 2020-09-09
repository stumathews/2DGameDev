#pragma once

class game_structure final : public event_subscriber
{
public:
	game_structure();
	std::shared_ptr<GameWorldData> g_pGameWorldData;
	void init_game_world_data() const;

	static bool load_media();
	static bool initialize(int screen_width, int screen_height);
	static void init3d_render_manager();
	static long get_tick_now();
	void update_active_elements();
	void world_update();
	void update();
	static void draw(float);
	static void spare_time(long);
	static bool init_sdl(int screenWidth, int screenHeight);
	static void cleanup_resources();
	void get_input();
	
	void player_update();

	void update_state();
	void update_logic_based_elements();
	vector<shared_ptr<Event>> process_event(std::shared_ptr<Event> evt) override;
	string get_subscriber_name() override;
};
