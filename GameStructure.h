#pragma once

class GameStructure 
{
public:
	GameStructure()
	{
		g_pGameWorldData = std::make_shared<GameWorldData>();
		init_game_world_data();
		
	}
	std::shared_ptr<GameWorldData> g_pGameWorldData;
	void init_game_world_data() const
	{
		g_pGameWorldData->bGameDone = false;
		g_pGameWorldData->bNetworkGame = false;
		g_pGameWorldData->bCanRender = true;
	}

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
	void get_input() const;
	
	void player_update() const;

	void update_state();
	void update_logic_based_elements();
};
