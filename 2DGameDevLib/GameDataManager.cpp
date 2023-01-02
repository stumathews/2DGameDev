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
	_eventManager->SubscribeToEvent(EventType::AddGameObjectToCurrentScene, this);
	_eventManager->SubscribeToEvent(EventType::GameObject, this);
}

std::vector<std::shared_ptr<Event>> GameDataManager::HandleEvent(const std::shared_ptr<Event> evt, unsigned long deltaMs)
{
	switch (evt->type)
	{
		case EventType::AddGameObjectToCurrentScene: AddToGameData(evt); break;
		case EventType::GameObject: RemoveFromGameData(evt); break;
	}
    return {};
}

GameDataManager::GameDataManager()
{	
	_eventManager = EventManager::Get();
	_eventFactory = EventFactory::Get();	
}

void GameDataManager::AddToGameData(const std::shared_ptr<gamelib::Event>& evt)
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

void GameDataManager::RemoveFromGameData(const std::shared_ptr<gamelib::Event> evt)
{
	const auto gameObjectEvent = dynamic_pointer_cast<GameObjectEvent>(evt);
	switch (gameObjectEvent->context)
	{
		case GameObjectEventContext::Remove: RemoveGameObject(gameObjectEvent->gameObject); break;
	}

	if (GameData::Get()->CountPickups() == 0 && !GameData::Get()->IsGameWon())
	{
		GameData::Get()->SetGameWon(true);
		_eventManager->RaiseEvent(std::make_shared<Event>(EventType::GameWon), this);
	}
}

void GameDataManager::RemoveGameObject(const std::shared_ptr<gamelib::GameObject>& gameObject) const
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
	_eventManager->Unsubscribe(gameObject->GetSubscriberId());
}

GameDataManager* GameDataManager::Instance = nullptr;
