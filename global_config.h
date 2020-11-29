#pragma once
#include <SDL_image.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

class global_config
{
public:	
	static const int tick_time_ms = 50; // 20 times a second = 50 milliseconds, 1 second is 20*50 = 1000 milliseconds
	static const int square_width = 30;
	static const int sprite_width = 100;
	static const int max_loops = 4;
	static const int screen_width = 800;
	static const int screen_height = 600;
	static const int move_interval = 1;
	static const auto frames_per_row = 3;
	static const auto frames_per_column = 3;
	static const auto player_init_pos_x = 100;
	static const auto player_init_pos_y = 100;
	static const bool verbose = true;
	static const bool print_debugging_text = false;
	static const bool use_3d_render_manager = false;
};
