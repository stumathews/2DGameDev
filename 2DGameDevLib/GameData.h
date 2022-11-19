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

	/// <summary>
	/// Get Static Game Data
	/// </summary>
	/// <returns></returns>
	static GameData* Get();

	/// <summary>
	/// Destroy the Game Data
	/// </summary>
	~GameData() { Instance = nullptr; }

	/// <summary>
	/// Add room to the list of game rooms
	/// </summary>
	/// <param name="room"></param>
	void AddRoom(std::shared_ptr<Room> room);

	/// <summary>
	/// Remove room from the list of game rooms
	/// </summary>
	/// <param name="room"></param>
	void RemoveRoom(std::shared_ptr<Room> room);

	/// <summary>
	/// Get Room By Index
	/// </summary>
	/// <param name="roomNumber"></param>
	/// <returns></returns>
	std::shared_ptr<Room> GetRoomByIndex(int roomNumber);

	/// <summary>
	/// Update the global game objects 
	/// </summary>
	/// <param name="gameObjects"></param>
	void SetGameObjects(std::vector<std::shared_ptr<gamelib::GameObject>>* gameObjects);

	/// <summary>
	/// Classify game objects
	/// </summary>
	void ClassifyGameObjects();

protected:
	GameData();
	static GameData* Instance;
private:
	
	/// <summary>
	/// Known rooms
	/// </summary>
	std::map<int, std::shared_ptr<Room>> _rooms;

	/// <summary>
	/// Pointer to game objects
	/// </summary>
	std::vector<std::shared_ptr<gamelib::GameObject>>* ptrGameObjects;
};

