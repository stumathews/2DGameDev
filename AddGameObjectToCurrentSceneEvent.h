#pragma once
#include "Event.h"
#include <memory>
#include "GameObject.h"

class AddGameObjectToCurrentSceneEvent : public Event
{
	std::shared_ptr<game_object> gameObject_;
public:
	AddGameObjectToCurrentSceneEvent(std::shared_ptr<game_object> gameObject) : Event(AddGameObjectToCurrentScene)
	{
		SetGameObject(gameObject);
	}

	void SetGameObject(std::shared_ptr<game_object> gameObject)
	{
		this->GameObject = gameObject;
	}
	std::shared_ptr<game_object> GetGameObject()
	{
		return GameObject;
	}

private:
	std::shared_ptr<game_object> GameObject;
};