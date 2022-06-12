#include "pch.h"
#include "GameData.h"
#include "room.h"
#include <memory>
#include <objects/GameObject.h>

GameData* GameData::Get()
{
    if (Instance == nullptr)
	{
		Instance = new GameData();
	}
	return Instance;
}

GameData::~GameData()
{
	Instance = nullptr;
}


void GameData::AddRoom(std::shared_ptr<Room> room)
{
	_rooms[room->GetRoomNumber()] = room;
}

void GameData::RemoveRoom(std::shared_ptr<Room> room)
{
	_rooms.erase(room->GetRoomNumber());
}

std::shared_ptr<Room> GameData::GetRoom(int roomNumber)
{
	return _rooms[roomNumber];
}

void GameData::SetGameObjects(std::vector<std::shared_ptr<gamelib::GameObject>>* gameObjects)
{
	_rooms.clear();
	ptrGameObjects = gameObjects;

	// classify objects
	for(auto object : *ptrGameObjects)
	{
		switch(object->GetGameObjectType())
		{
			case gamelib::GameObjectType::Room:
				std::shared_ptr<Room> room = std::dynamic_pointer_cast<Room>(object);
				AddRoom(room);
				break;
		}
	}
}


GameData::GameData()
{
	ptrGameObjects = nullptr;
}

GameData* GameData::Instance = nullptr;
