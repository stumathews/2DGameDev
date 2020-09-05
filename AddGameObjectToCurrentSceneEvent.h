#pragma once
#include "Event.h"
#include <memory>
#include "GameObject.h"

class AddGameObjectToCurrentSceneEvent : public Event
{
	std::shared_ptr<GameObject> gameObject_;
public:
	AddGameObjectToCurrentSceneEvent(std::shared_ptr<GameObject> gameObject) : Event(AddGameObjectToCurrentScene)
	{
		SetGameObject(gameObject);
	}

	void SetGameObject(std::shared_ptr<GameObject> gameObject)
	{
		this->GameObject = gameObject;
	}
	std::shared_ptr<GameObject> GetGameObject()
	{
		return GameObject;
	}

private:
	std::shared_ptr<GameObject> GameObject;
};