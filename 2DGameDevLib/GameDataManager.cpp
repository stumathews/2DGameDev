#include "pch.h"
#include "GameDataManager.h"
#include <events/Event.h>
#include <objects/GameObject.h>
#include "Room.h"
#include <events/AddGameObjectToCurrentSceneEvent.h>
#include <events/GameObjectEvent.h>

#include "EventNumber.h"
#include "pickup.h"

using namespace std;
using namespace gamelib;

void GameDataManager::Initialize(const bool isNetworkGame = false)
{
	SubscribeToEvent(AddGameObjectToCurrentSceneEventId);
	SubscribeToEvent(GameObjectTypeEventId);
	GameData::Get()->IsNetworkGame = isNetworkGame;
	GameData::Get()->IsGameDone = false;
	GameData::Get()->IsNetworkGame = false;
	GameData::Get()->CanDraw = true;
	GameWorldData.CanDraw = GameData::Get()->CanDraw;
	GameWorldData.IsNetworkGame = GameData::Get()->IsNetworkGame;
	GameWorldData.IsGameDone = GameData::Get()->IsGameDone;
}

GameDataManager::GameDataManager()
{
	eventManager = EventManager::Get();
	eventFactory = EventFactory::Get();
}

GameDataManager* GameDataManager::Get()
{
	if (instance == nullptr) { instance = new GameDataManager(); }
	return instance;
}

ListOfEvents GameDataManager::HandleEvent(const std::shared_ptr<Event> event, unsigned long deltaMs)
{
	if (event->Id == AddGameObjectToCurrentSceneEventId)
	{
		AddToGameData(dynamic_pointer_cast<AddGameObjectToCurrentSceneEvent>(event));
	}

	if (event->Id == GameObjectTypeEventId)
	{
		RemoveFromGameData(dynamic_pointer_cast<GameObjectEvent>(event));
	}
	return {};
}

inline std::string GameDataManager::GetSubscriberName()
{
	return "GameDataManager";
}

void GameDataManager::AddToGameData(const shared_ptr<AddGameObjectToCurrentSceneEvent>& event) const
{
	const auto gameObject = dynamic_pointer_cast<AddGameObjectToCurrentSceneEvent>(event)->GetGameObject();

	if (gameObject->GetGameObjectType() == GameObjectType::GameDefined)
	{
		if (gameObject->Type == "Room")
		{
			GameData()->AddRoom(std::dynamic_pointer_cast<Room>(gameObject));
		}
		if (gameObject->Type == "Enemy")
		{
			GameData()->AddEnemy(std::dynamic_pointer_cast<Enemy>(gameObject));
		}
	}
	else
	{
		if (gameObject->GetGameObjectType() == GameObjectType::Pickup)
		{
			GameData()->AddPickup(std::dynamic_pointer_cast<Pickup>(gameObject));
		}
	}

	GameData()->AddGameObject(gameObject);
}

void GameDataManager::RemoveFromGameData(const std::shared_ptr<GameObjectEvent>& event)
{
	if(event->Context == GameObjectEventContext::Remove)
	{
		RemoveGameObject(event->Object);
	}

	if (GameData::Get()->CountPickups() == 0 && !GameData::Get()->IsGameWon())
	{
		GameData::Get()->SetGameWon(true);
		eventManager->RaiseEvent(std::make_shared<Event>(GameWonEventId), this);
	}
}

void GameDataManager::RemoveGameObject(const std::shared_ptr<GameObject>& gameObject) const
{
	if (gameObject->Type == "Room")
	{
		GameData::Get()->RemoveRoom(dynamic_pointer_cast<Room>(gameObject));
	}

	if (gameObject->Type == "Pickup")
	{
		GameData::Get()->RemovePickup(dynamic_pointer_cast<Pickup>(gameObject));
	}

	if (gameObject->Type == "Enemy")
	{
		GameData::Get()->RemoveEnemy(dynamic_pointer_cast<Enemy>(gameObject));
	}

	GameData::Get()->RemoveGameObject(gameObject);
	GameData::Get()->RemoveExpiredReferences();
	eventManager->Unsubscribe(gameObject->GetSubscriberId());
}

GameDataManager* GameDataManager::instance = nullptr;
