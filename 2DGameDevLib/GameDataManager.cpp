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

void GameDataManager::Initialize()
{
	SubscribeToEvent(AddGameObjectToCurrentSceneEventId);
	SubscribeToEvent(GameObjectTypeEventId);
}

GameDataManager::GameDataManager()
{	
	eventManager = EventManager::Get();
	eventFactory = EventFactory::Get();	
}

std::vector<std::shared_ptr<Event>> GameDataManager::HandleEvent(const std::shared_ptr<Event> evt, unsigned long deltaMs)
{
	if(evt->Id == AddGameObjectToCurrentSceneEventId)
	{
		AddToGameData(evt); 
	}

	if(evt->Id == GameObjectTypeEventId)
	{
		RemoveFromGameData(evt);
	}
    return {};
}

void GameDataManager::AddToGameData(const std::shared_ptr<Event>& evt) const
{
	const auto object = dynamic_pointer_cast<AddGameObjectToCurrentSceneEvent>(evt)->GetGameObject();

	if (object->GetGameObjectType() == GameObjectType::GameDefined)
	{
		if (object->Type == "Room")
		{
			GameData()->AddRoom(std::dynamic_pointer_cast<Room>(object));
		}
		if (object->Type == "Enemy")
		{
			GameData()->AddEnemy(std::dynamic_pointer_cast<Enemy>(object));
		}
	}
	else 
	{
		if (object->GetGameObjectType() == GameObjectType::Pickup)
		{
			GameData()->AddPickup(std::dynamic_pointer_cast<Pickup>(object));
		}
	}

	GameData()->AddGameObject(object);
}

void GameDataManager::RemoveFromGameData(const std::shared_ptr<Event>& evt)
{
	const auto gameObjectEvent = dynamic_pointer_cast<GameObjectEvent>(evt);
	switch (gameObjectEvent->Context)  // NOLINT(clang-diagnostic-switch-enum)
	{
		case GameObjectEventContext::Remove: RemoveGameObject(gameObjectEvent->Object); break;
		default: /* Do Nothing */;
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
