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
#include "Single.h"
#include "LevelGenerator.h"

using namespace std;
void game_loop();
void unload();
bool load_content();
bool initialize();


int main(int argc, char *args[])
{
	if (!initialize() || !load_content)
		return -1;
	load_content();	
	game_loop();	
	unload();	
	return 0;
}

bool initialize()
{
	const auto screen_width = Single<GlobalConfig>().ScreenWidth;
	const auto screen_height = Single<GlobalConfig>().ScreenHeight;
	
	if (!GameStructure::initialize(screen_width, screen_height))
		return false;

	event_manager::get_instance().register_event(std::make_shared<scene_changed_event>(1)); // Trigger the first level by kicking the event manager
	return true;
}


bool load_content()
{
	for (const auto& square : level_generator::generate_level())
	{
		std::shared_ptr<GameObject> gameObject = std::dynamic_pointer_cast<Square>(square);
		//gameObject->SubScribeToEvent(PlayerMovedEventType);
		//gameObject->RaiseEvent(std::make_shared<AddGameObjectToCurrentSceneEvent>(&gameObject));		
	}

	Player::add_player_to_game();
	return true;
}

void game_loop()
{
	auto tickCountAtLastCall = GameStructure::get_tick_now();
	auto max_loops = Single<GlobalConfig>().MaxLoops;

	// MAIN GAME LOOP!!
	while (!Single<GameStructure>().g_pGameWorldData->bGameDone) 
	{
		const auto new_time =  GameStructure::get_tick_now();
		auto frame_ticks = 0;  // Number of ticks in the update call	
		auto num_loops = 0;  // Number of loops ??
		auto ticks_since = new_time - tickCountAtLastCall;

		// New frame, happens consistently every 50 milliseconds. Ie 20 times a second.
		// 20 times a second = 50 milliseconds
		// 1 second is 20*50 = 1000 milliseconds
		while (ticks_since > GlobalConfig::TICK_TIME_MS && num_loops < max_loops)
		{
			Single<GameStructure>().update();		
			tickCountAtLastCall += GlobalConfig::TICK_TIME_MS; // tickCountAtLastCall is now been +Single<GlobalConfig>().TickTime more since the last time. update it
			frame_ticks += GlobalConfig::TICK_TIME_MS; num_loops++;
			ticks_since = new_time - tickCountAtLastCall;
		}

		GameStructure::spare_time(frame_ticks); // handle player input, general housekeeping (Event Manager processing)

		if (Single<GameStructure>().g_pGameWorldData->bNetworkGame || ticks_since <= GlobalConfig::TICK_TIME_MS)
		{
			if (Single<GameStructure>().g_pGameWorldData->bCanRender)
			{
				const auto percent_outside_frame = static_cast<float>(ticks_since / GlobalConfig::TICK_TIME_MS) * 100; // NOLINT(bugprone-integer-division)				
				GameStructure::draw(percent_outside_frame);
			}
		}
		else
		{
			tickCountAtLastCall = new_time - GlobalConfig::TICK_TIME_MS;
		}
	}
	std::cout << "Game done" << std::endl;
}

void unload()
{	
	GameStructure::cleanup_resources();
}








