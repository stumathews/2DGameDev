#include <SDL.h>
#include <Windows.h>
#include <iostream>
#include "Common.h"
#include "GameObject.h"
#include <vector>
#include "ResourceManager.h"
#include "SceneChangedEvent.h"
#include "DoLogicUpdateEvent.h"
#include "RenderManager3D.h"
#include "AddGameObjectToCurrentSceneEvent.h"
#include "Square.h"
#include "PlayerComponent.h"
#include "Player.h"
#include "constants.h"
#include "GlobalConfig.h"
#include "GameStructure.h"
#include "singleton.h"
#include "LevelGenerator.h"
#include "Logger.h"

using namespace std;
void game_loop();
void unload();
bool load_content();
bool initialize();


int main(int argc, char *args[])
{

	if (!initialize() || !load_content())
		return -1;	
	
	game_loop();
	
	unload();	
	return 0;
}

bool initialize()
{
	const auto screen_width = GlobalConfig::ScreenWidth;
	const auto screen_height = GlobalConfig::ScreenHeight;
	
	if (!game_structure::initialize(screen_width, screen_height))
		return false;

	
	return true;
}


bool load_content()
{
	logger::log_message("load_content()");
	for (const auto& sq : level_generator::generate_level())
	{
		//std::shared_ptr<game_object> game_object = std::dynamic_pointer_cast<square>(sq);
		//game_object->subscribe_to_event(PlayerMovedEventType);
		//game_object->raise_event(std::make_shared<AddGameObjectToCurrentSceneEvent>(game_object));		
	}

	Player::add_player_to_game();
	return true;
}

void game_loop()
{
	auto tick_count_at_last_call = game_structure::get_tick_now();
	const auto max_loops = GlobalConfig::MaxLoops;

	// MAIN GAME LOOP!!
	while (!singleton<game_structure>().g_pGameWorldData->bGameDone) 
	{
		const auto new_time =  game_structure::get_tick_now();
		auto frame_ticks = 0;  // Number of ticks in the update call	
		auto num_loops = 0;  // Number of loops ??
		auto ticks_since = new_time - tick_count_at_last_call;

		// New frame, happens consistently every 50 milliseconds. Ie 20 times a second.
		// 20 times a second = 50 milliseconds
		// 1 second is 20*50 = 1000 milliseconds
		while (ticks_since > GlobalConfig::TICK_TIME_MS && num_loops < max_loops)
		{
			singleton<game_structure>().update();		
			tick_count_at_last_call += GlobalConfig::TICK_TIME_MS; // tickCountAtLastCall is now been +Single<GlobalConfig>().TickTime more since the last time. update it
			frame_ticks += GlobalConfig::TICK_TIME_MS; num_loops++;
			ticks_since = new_time - tick_count_at_last_call;
		}

		game_structure::spare_time(frame_ticks); // handle player input, general housekeeping (Event Manager processing)

		if (singleton<game_structure>().g_pGameWorldData->bNetworkGame || ticks_since <= GlobalConfig::TICK_TIME_MS)
		{
			if (singleton<game_structure>().g_pGameWorldData->bCanRender)
			{
				const auto percent_outside_frame = static_cast<float>(ticks_since / GlobalConfig::TICK_TIME_MS) * 100; // NOLINT(bugprone-integer-division)				
				game_structure::draw(percent_outside_frame);
			}
		}
		else
		{
			tick_count_at_last_call = new_time - GlobalConfig::TICK_TIME_MS;
		}
	}
	std::cout << "Game done" << std::endl;
}

void unload()
{	
	game_structure::cleanup_resources();
}








