#pragma once
#include <memory>
#include <map>
#include <vector>

namespace gamelib 
{
	class GameObject;
}

class Room;

class GameData
{
public:
	static GameData* Get();
	~GameData();	

	void AddRoom(std::shared_ptr<Room> room);
	void RemoveRoom(std::shared_ptr<Room> room);
	std::shared_ptr<Room> GetRoom(int roomNumber);

	void SetGameObjects(std::vector<std::shared_ptr<gamelib::GameObject>>* gameObjects);

	protected:
	GameData();
	static GameData* Instance;
private:
	// Rooms
	std::map<int, std::shared_ptr<Room>> _rooms;
	std::vector<std::shared_ptr<gamelib::GameObject>>* ptrGameObjects;
};

