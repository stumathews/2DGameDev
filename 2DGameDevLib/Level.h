#pragma once
#include <string>
#include <vector>
#include <memory>

namespace gamelib
{
	class GameObject;	
	class Pickup;
}

namespace tinyxml2
{
	class XMLNode;
}

class Player;
class Room;

class Level
{
public:
	explicit Level(const std::string& filename);
	Level();
	void Load();
	std::shared_ptr<gamelib::GameObject> ParseObject(tinyxml2::XMLNode* pSpriteChild, const std::shared_ptr<Room>& room) const;
	static void ParseProperty(tinyxml2::XMLNode* pObjectChild, const std::shared_ptr<gamelib::GameObject>& go);
	std::vector<std::shared_ptr<Room>> Rooms;
	std::vector<std::shared_ptr<gamelib::Pickup>> Pickups;
	std::shared_ptr<Player> Player1;
	std::string FileName;
	int NumCols;
	int NumRows;
	unsigned int ScreenWidth;
	unsigned int ScreenHeight;
	[[nodiscard]] bool IsAutoLevel() const { return isAutoLevel || FileName.empty(); }
	[[nodiscard]] bool IsAutoPopulatePickups() const { return isAutoPopulatePickups; }
private:
	bool isAutoLevel;
	bool isAutoPopulatePickups;
};


