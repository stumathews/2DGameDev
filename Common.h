#ifndef COMMON_H
#define COMMON_H

#include <SDL.h>
#include <vector>
#include <memory>
#include "event_manager.h"
#include "game_object.h"
#include <iostream>
#include "Logger.h"
#include "Common.h"

inline bool succeeded(const bool result, string message = "")
{
	const auto is = result == true;
	if(is == true)
		logger::log_message(message);
	return is;
}

struct game_world_data 
{
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;
	
	bool is_game_done = false;
	bool is_network_game = false;
	bool can_render = true;
		
	// List of game objects
	std::vector<std::shared_ptr<game_object>> game_objects;
};

inline void log_message(const string &message, const bool be_verbose = global_config::verbose)
{
	logger::log_message(message, be_verbose);
}

inline bool run_and_log(const string &message, bool verbose, const std::function<bool()>& action)
{
	log_message(message);
	return action();
}

template <typename ENUM>
auto as_integer(ENUM const value)
{
	return static_cast<typename std::underlying_type<ENUM>::type>(value);
}

#endif