#pragma once

class GameStructure 
{
public:
	GameStructure()
	{
		g_pGameWorldData = std::shared_ptr<GameWorldData>(new GameWorldData());
		InitGameWorldData();
		
	}
	std::shared_ptr<GameWorldData> g_pGameWorldData;
	void InitGameWorldData()
	{
		g_pGameWorldData->bGameDone = 0;
		g_pGameWorldData->bNetworkGame = 0;
		g_pGameWorldData->bCanRender = true;
	}

	bool loadMedia();
	long ticks();
	void update_active_elements();
	void world_update();
	void Update();
    void Draw(float);
    void SpareTime(long);
	void npc_render_data();
	void npc_pack_data();
	void npc_animate();
	void npc_select_visible_subset();
	void NPC_Presentation();
	void player_render_data();
	void player_data_pack();
	void player_animate();
	void Player_Presentation();
	void world_send_audio_data_to_audio_hardware();
	void world_pack_audio_data();
	void world_select_audible_sound_sources();
	void world_pack_geometry();
	void world_render_geometry();
	void world_elements_clip();
	void world_elements_cull();
	void world_elements_occulude();
	void world_select_visible_graphic_elements();
	void world_select_resolution();
	void send_audio_to_hardware();
	void send_geometry_to_hardware();
	void World_Presentation();
	bool InitSDL(int screenWidth, int screenHeight);
	void CleanupResources();
	void sense_player_input();
	void determine_restrictions();
	void update_player_state();
	void player_update();
	void pre_select_active_zones();
	void update_passive_elements();
	void logic_sort_according_to_relevance();
	void execute_control_mechanism();
	void update_state();
	void update_logic_based_elements();
	void update_world_data_structure();
	void decision_engine();
	void sense_restrictions();
	void sense_internal_state_and_goals();
	void ai_sort_according_to_gamplay_relevance();
	void update_ai_based_elements();
};
