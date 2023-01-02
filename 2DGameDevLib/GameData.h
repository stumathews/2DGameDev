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
	void AddRoom(const std::shared_ptr<Room>& room);
	void RemoveRoom(const std::shared_ptr<Room>& room);
	void AddPickup(std::shared_ptr<gamelib::Pickup> pickup);
	void RemovePickup(const std::shared_ptr<gamelib::Pickup>& pickup);
	
	std::shared_ptr<Room> GetRoomByIndex(int roomNumber);
	[[nodiscard]] std::shared_ptr<Player> GetPlayer() const;
	[[nodiscard]] unsigned int CountPickups() const {	return _pickups.size(); }
	void SetGameWon(const bool yesNo) { isGameWon = yesNo; }
	[[nodiscard]] bool IsGameWon() const { return isGameWon; }
	void AddGameObject(const std::shared_ptr<gamelib::GameObject>& gameObject);
	void RemoveGameObject(const std::shared_ptr<gamelib::GameObject>& gameObject);

	void RemoveExpiredReferences();

	std::vector<std::weak_ptr<gamelib::GameObject>> GameObjects;
protected:
	GameData();
	static GameData* Instance;
private:
	static bool IsSameId(const std::weak_ptr<gamelib::GameObject>& obj, const std::shared_ptr<gamelib::GameObject>&
	                     other);
	bool isGameWon;
	std::map<int, std::weak_ptr<Room>> _rooms;
	std::vector<std::weak_ptr<gamelib::Pickup>> _pickups;
	
};

