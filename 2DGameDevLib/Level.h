#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Room.h"

class Level
{
public:

	/// <summary>
	/// Create a Level
	/// </summary>
	/// <param name="filename">file associated with this level</param>
	Level(std::string filename);

	/// <summary>
	/// Load the level
	/// </summary>
	void Load();

	/// <summary>
	/// The list of rooms in th level
	/// </summary>
	std::vector<std::shared_ptr<Room>> Rooms;

	/// <summary>
	/// Filename asosciated with this level
	/// </summary>
	std::string FileName;

	/// <summary>
	/// Number of columns in this level
	/// </summary>
	int NumCols;

	/// <summary>
	/// Number of rows in this level
	/// </summary>
	int NumRows;
};