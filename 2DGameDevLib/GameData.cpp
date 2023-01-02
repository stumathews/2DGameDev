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

void GameData::AddRoom(const std::shared_ptr<Room>& room) { _rooms[room->GetRoomNumber()] = room; }
void GameData::RemoveRoom(const std::shared_ptr<Room>& room) { _rooms.erase(room->GetRoomNumber()); }
void GameData::RemovePickup(const std::shared_ptr<Pickup>& pickup) { _pickups.erase(remove_if(begin(_pickups), end(_pickups), [=](const weak_ptr<GameObject>
	& obj) { return IsSameId(obj, pickup); }), _pickups.end()); }
void GameData::RemoveGameObject(const std::shared_ptr<GameObject>& gameObject) { GameObjects.erase(remove_if(begin(GameObjects), end(GameObjects), [=](const weak_ptr<GameObject>
	& obj) { return IsSameId(obj, gameObject); }), GameObjects.end()); }
void GameData::RemoveExpiredReferences() { GameObjects.erase(remove_if(begin(GameObjects), end(GameObjects), [&](const weak_ptr<GameObject>
                                                                       & obj) { return obj.expired(); }), end(GameObjects)); }
bool GameData::IsSameId(const weak_ptr<GameObject>& obj, const std::shared_ptr<GameObject>& other) { return !obj.expired() && obj.lock()->Id == other->Id; }

std::shared_ptr<Room> GameData::GetRoomByIndex(const int roomNumber) { return _rooms[roomNumber].lock(); }
std::shared_ptr<Player> GameData::GetPlayer() const { return dynamic_pointer_cast<Player>(player.lock()); }

GameData* GameData::Get() { if (Instance == nullptr) { Instance = new GameData(); } return Instance; }

void GameData::AddPickup(const std::shared_ptr<Pickup> pickup) 
{
	if (std::find_if(_pickups.begin(), _pickups.end(), [&](const std::weak_ptr<GameObject>& gameObject) { return !gameObject.expired() && gameObject.lock()->Id == pickup->Id;}) == _pickups.end())
	{
		_pickups.push_back(pickup);		
	}
	if (!_pickups.empty()) { SetGameWon(false); }
}

void GameData::AddGameObject(const std::shared_ptr<GameObject>& gameObject)
{
	if (std::find_if(GameObjects.begin(), GameObjects.end(), [=](const weak_ptr<GameObject>& obj) { return IsSameId(obj, gameObject); }) == GameObjects.end())
	{
		GameObjects.push_back(gameObject);
	}
}
