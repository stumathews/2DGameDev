#include "pch.h"
#include "LevelGenerator.h"
#include <algorithm>
#include <iostream>
#include <random>
#include <vector>
#include <util/SettingsManager.h>


using namespace std;
using namespace gamelib;

LevelGenerator::LevelGenerator(int screenWidth, int screenHeight, int rows, int columns, bool removeRandomSides)
	: screenWidth(screenWidth), screenHeight(screenHeight), rows(rows), columns(columns), removeRandomSides(removeRandomSides)
{

}

/// <summary>
/// Generates the Rooms in the Level
/// </summary>
vector<shared_ptr<Room>> LevelGenerator::Generate()
{
	
	// caclulate the dimensions of a room (a square with 4 sides/walls)
	const auto square_width = screenWidth / columns; 
	const auto square_height = screenHeight / rows;
		
	// List of Rooms generated
	vector<shared_ptr<Room>> rooms;

	// Generate Row x Col Rooms for the Maze
	auto count = 0;

	// Generate n rows
	for(auto row = 0; row < rows; row++)
	{
		// generate n columns in this row
		for(auto col = 0; col < columns; col++)
		{			
			// each room has a unique room number
			auto number = count++;

			// rooms do not support moving
			auto support_move_logic = false;
				
			// Create room
			auto room = std::shared_ptr<Room>(new Room(number, col * square_width, row * square_height, square_width, square_height, false));				
				
			// Set room tag to room number
			room->SetTag(std::to_string(number));

			// Add to list of rooms in game
			rooms.push_back(room);			
		}
	}

	const auto total_rooms = static_cast<int>(rooms.size());	
		
	// Go through each room and remove/disable some walls
	// Determine which faces/edges can be removed, based on the bounds of the grid i.e within rows x cols of board
	for(auto i = 0; i < total_rooms-1; i++)
	{
		const auto next_index = i + 1;
		const auto prevIndex = i - 1;

		if(next_index >= total_rooms)
			break;

		// Collect contextual information about room relative to others around it

		// We are operating under zero-based logic from here on out:

		const auto this_row = (int)ceil(i/columns);
		const auto last_column = (int)(this_row+1 * columns)-1;
		const auto this_col = i % columns;

		const auto room_above_index = i - columns;
		const auto room_below_index = i + columns;
		const auto room_left_index = i - 1;
		const auto room_right_index = i + 1;

		const auto can_remove_above = room_above_index >= 0;
		const auto can_remove_below = room_below_index < total_rooms;
		const auto can_remove_left = this_col-1 >= 1;
		const auto can_remove_right = this_col+1 <= columns;

		auto &current_room = rooms[i];
		auto &next_room = rooms[next_index];

		// List of sides that are removable (some cannot be removed if they are on the bounds of the board/game)
		vector<Side> removableSides;		

		// Set information about rooms around this one
		current_room->SetSoroundingRooms(room_above_index, room_right_index, room_below_index, room_left_index);

		// is possible to remove top wall of this room?
		if(can_remove_above)
		{
			removableSides.push_back(Side::Top);
		}

		// is possible to remove bottom wall of this room?
		if(can_remove_below)
		{
			removableSides.push_back(Side::Bottom);
		}

		// is possible to remove left wall of this room?
		if(can_remove_left)
		{
			removableSides.push_back(Side::Left);
		}

		// is possible to remove right wall of this room?
		if(can_remove_right)
		{
			removableSides.push_back(Side::Right);
		}

		// Choose a 1 random element wall to remove from possible sides 
		vector<Side> random_sides;
		uint n_sample = 1;			
		std::sample(begin(removableSides),
				    end(removableSides),
				    std::back_inserter(random_sides), 
				    n_sample, 
				    std::mt19937 { std::random_device{}() });

		const auto remove_sides_randomly = removeRandomSides;
			
		// Removes one side randonly
		if(remove_sides_randomly)
		{				
			auto aSide = random_sides.front();

			if(aSide == Side::Top && can_remove_above)
			{
				// remove the top side of this room
				current_room->RemoveWallZeroBased(Side::Top); 
					
				auto roomAbove = rooms[current_room->GetNeighbourIndex(Side::Top)];
					
				// remove the bottom of the room above 
				roomAbove->RemoveWallZeroBased(Side::Bottom);

				next_room->RemoveWallZeroBased(Side::Bottom);					
			}

			if(aSide == Side::Right && can_remove_right)
			{
				// Remove the right side of this room
				current_room->RemoveWallZeroBased(Side::Right);
				auto roomToLeft = rooms[current_room->GetNeighbourIndex(Side::Right)];

				// Remove room on right's left wall
				roomToLeft->RemoveWallZeroBased(Side::Left);

				next_room->RemoveWallZeroBased(Side::Left);
			}

			if(aSide == Side::Bottom && can_remove_below)
			{					
				// Remove the bottom side of this room
				current_room->RemoveWallZeroBased(Side::Bottom);
					
				// Remove the room belows top wall
				auto roomBelow = rooms[current_room->GetNeighbourIndex(Side::Bottom)];
				roomBelow->RemoveWallZeroBased(Side::Top);

				next_room->RemoveWallZeroBased(Side::Top);
			}

			if(aSide == Side::Left && can_remove_left)
			{
				// Remove the left wall on this room
				current_room->RemoveWallZeroBased(Side::Left);				
				auto &prev = rooms[prevIndex]; 

				// Remove the room on left side's right wall
				auto roomOnLeft = rooms[current_room->GetNeighbourIndex(Side::Left)];
				roomOnLeft->RemoveWallZeroBased(Side::Right);

				prev->RemoveWallZeroBased(Side::Right);
			}				
		}
	}

	return rooms;
}
