#include "pch.h"
#include "Level.h"
#include "tinyxml2.h"
#include "Room.h"
#include <util/SettingsManager.h>
#include "Rooms.h"

using namespace tinyxml2;
using namespace std;
using namespace gamelib;

Level::Level(std::string filename)
{
	FileName = filename;
}

void Level::Load()
{
	tinyxml2::XMLDocument doc;

	// Load file
	doc.LoadFile(FileName.c_str());
	if (doc.ErrorID() == 0)
	{
		auto* scene = doc.FirstChildElement("level");
		auto cols = static_cast<int>(std::atoi(scene->ToElement()->Attribute("cols")));			
		auto rows = static_cast<int>(std::atoi(scene->ToElement()->Attribute("rows")));		
		const auto screenWidth = SettingsManager::Get()->GetInt("global", "screen_width");
		const auto screenHeight = SettingsManager::Get()->GetInt("global", "screen_height");
		
		NumCols = cols;
		NumRows = rows;

		// List of Rooms generated
		
		for (auto roomNode = scene->FirstChild(); roomNode; roomNode = roomNode->NextSibling()) //  <room ...>
		{
			auto roomElement = roomNode->ToElement();
			auto number = static_cast<int>(std::atoi(roomElement->ToElement()->Attribute("number")));
			auto top = string(roomElement->ToElement()->Attribute("top"));
			auto right = string(roomElement->ToElement()->Attribute("right"));
			auto bottom = string(roomElement->ToElement()->Attribute("bottom"));
			auto left = string(roomElement->ToElement()->Attribute("left"));

			auto row = number / cols; // row for this roomNumber
			auto rowCol0 = row * cols;
			auto col = number - rowCol0; // col for this roomNumber

			const auto square_width = screenWidth / cols;
			const auto square_height = screenHeight / rows;

			auto room = std::shared_ptr<Room>(new Room(number, col * square_width, row * square_height, square_width, square_height, false));
			
			auto SetWall = [&](std::string isSideVisible, Side side, std::shared_ptr<Room> room) -> void
			{
				if (isSideVisible == "True")
					room->AddWall(side);
				else
					room->RemoveWall(side);
			};

			SetWall(right, Side::Right, room);
			SetWall(left, Side::Left, room);
			SetWall(top, Side::Top, room);
			SetWall(bottom, Side::Bottom, room);

			// Set room tag to room number
			room->SetTag(std::to_string(number));

			// Add to list of rooms in game
			Rooms.push_back(room);
		}

		Rooms::ConfigureRooms(rows, cols, Rooms);
	}
}
