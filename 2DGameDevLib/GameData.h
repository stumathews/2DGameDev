#pragma once
#include <memory>
#include <map>
#include <vector>
#include <objects/GameWorldData.h>

namespace gamelib 
{
	class GameObject;
	class Pickup;
}

class Room;
class Player;


class GameData : public gamelib::GameWorldData
{
public:
	static GameData* Get();
	
	~GameData() { Instance = nullptr; }
	void AddRoom(std::shared_ptr<Room> room);
	void RemoveRoom(std::shared_ptr<Room> room);
	void AddPickup(std::shared_ptr<gamelib::Pickup> pickup);
	void RemovePickup(std::shared_ptr<gamelib::Pickup> pickup);
	
	std::shared_ptr<Room> GetRoomByIndex(int roomNumber);
	std::shared_ptr<Player> GetPlayer();
	unsigned int CountPickups() { 
		return _pickups.size();
	}
	void SetGameWon(bool yesno) { isGameWon = yesno; }
	bool IsGameWon() { return isGameWon; }
	void AddGameObject(std::shared_ptr<gamelib::GameObject> gameObject);
	void RemoveGameObject(std::shared_ptr<gamelib::GameObject> gameObject);

	void RemoveExpiredReferences();

	std::vector<std::weak_ptr<gamelib::GameObject>> GameObjects;
protected:
	GameData();
	static GameData* Instance;
private:
	bool IsSameId(std::weak_ptr<gamelib::GameObject> obj, std::shared_ptr<gamelib::GameObject> other);
	bool isGameWon;
	std::map<int, std::weak_ptr<Room>> _rooms;
	std::vector<std::weak_ptr<gamelib::Pickup>> _pickups;
	
};

