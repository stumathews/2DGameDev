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

std::shared_ptr<Room> GameData::GetRoomByIndex(int roomNumber)
{
	return _rooms[roomNumber];
}

void GameData::SetGameObjects(std::vector<std::shared_ptr<gamelib::GameObject>>* gameObjects)
{
	_rooms.clear();
	ptrGameObjects = gameObjects;

	ClassifyGameObjects();
}

void GameData::ClassifyGameObjects()
{
	for (auto& object : *ptrGameObjects)
	{		
		switch (object->GetGameObjectType())
		{
		case gamelib::GameObjectType::Room:
			AddRoom(std::dynamic_pointer_cast<Room>(object));
			break;
		}
	}
}


GameData::GameData()
{
	ptrGameObjects = nullptr;
}

GameData* GameData::Instance = nullptr;
