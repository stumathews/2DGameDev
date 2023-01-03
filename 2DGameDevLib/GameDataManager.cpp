#include "pch.h"
#include "GameDataManager.h"
#include <events/Event.h>
#include <objects/GameObject.h>
#include "Room.h"
#include <events/AddGameObjectToCurrentSceneEvent.h>
#include <events/GameObjectEvent.h>
#include "pickup.h"

using namespace std;
using namespace gamelib;

void GameDataManager::Initialize()
{
	eventManager->SubscribeToEvent(EventType::AddGameObjectToCurrentScene, this);
	eventManager->SubscribeToEvent(EventType::GameObject, this);
}

std::vector<std::shared_ptr<Event>> GameDataManager::HandleEvent(const std::shared_ptr<Event> evt, unsigned long deltaMs)
{
	switch (evt->Type)  // NOLINT(clang-diagnostic-switch-enum)
	{
		case EventType::AddGameObjectToCurrentScene: AddToGameData(evt); break;
		case EventType::GameObject: RemoveFromGameData(evt); break;
		default: /* Do Nothing */ ;
	}
    return {};
}

GameDataManager::GameDataManager()
{	
	eventManager = EventManager::Get();
	eventFactory = EventFactory::Get();	
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
		case GameObjectEventContext::Remove: RemoveGameObject(gameObjectEvent->GameObject); break;
		default: /* Do Nothing */;
	}

	if (GameData::Get()->CountPickups() == 0 && !GameData::Get()->IsGameWon())
	{
		GameData::Get()->SetGameWon(true);
		eventManager->RaiseEvent(std::make_shared<Event>(EventType::GameWon), this);
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

	GameData::Get()->RemoveGameObject(gameObject);
	GameData::Get()->RemoveExpiredReferences();
	eventManager->Unsubscribe(gameObject->GetSubscriberId());
}

GameDataManager* GameDataManager::Instance = nullptr;
