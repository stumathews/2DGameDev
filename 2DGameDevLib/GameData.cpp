#include "pch.h"
#include "GameData.h"
#include "room.h"
#include <memory>
#include <objects/GameObject.h>
#include <vector>
#include "pickup.h"
#include "Player.h"

using namespace gamelib;
using namespace std;

GameData::GameData(): isGameWon(false) { }
GameData* GameData::Instance = nullptr;

void GameData::AddRoom(std::shared_ptr<Room> room) { _rooms[room->GetRoomNumber()] = room; }
void GameData::RemoveRoom(std::shared_ptr<Room> room) { _rooms.erase(room->GetRoomNumber()); }
void GameData::RemovePickup(std::shared_ptr<Pickup> pickup) { _pickups.erase(remove_if(begin(_pickups), end(_pickups), [=](weak_ptr<GameObject> obj) { return IsSameId(obj, pickup); }), _pickups.end()); }
void GameData::RemoveGameObject(std::shared_ptr<GameObject> gameObject) { GameObjects.erase(remove_if(begin(GameObjects), end(GameObjects), [=](weak_ptr<GameObject> obj) { return IsSameId(obj, gameObject); }), GameObjects.end()); }
void GameData::RemoveExpiredReferences() { GameObjects.erase(remove_if(begin(GameObjects), end(GameObjects), [&](weak_ptr<GameObject> obj) { return obj.expired(); }), end(GameObjects)); }
bool GameData::IsSameId(weak_ptr<GameObject> obj, std::shared_ptr<GameObject> other) { return !obj.expired() && obj.lock()->Id == other->Id; }

std::shared_ptr<Room> GameData::GetRoomByIndex(int roomNumber) { return _rooms[roomNumber].lock(); }
std::shared_ptr<Player> GameData::GetPlayer() { return dynamic_pointer_cast<Player>(player.lock()); }

GameData* GameData::Get() { if (Instance == nullptr) { Instance = new GameData(); } return Instance; }

void GameData::AddPickup(std::shared_ptr<Pickup> pickup) 
{
	if (std::find_if(_pickups.begin(), _pickups.end(), [&](std::weak_ptr<GameObject> gameObject) { return !gameObject.expired() && gameObject.lock()->Id == pickup->Id;}) == _pickups.end())
	{
		_pickups.push_back(pickup);		
	}
	if (_pickups.size() > 0) { SetGameWon(false); }
}

void GameData::AddGameObject(std::shared_ptr<GameObject> gameObject)
{
	if (std::find_if(GameObjects.begin(), GameObjects.end(), [=](weak_ptr<GameObject> obj) { return IsSameId(obj, gameObject); }) == GameObjects.end())
	{
		GameObjects.push_back(gameObject);
	}
}
