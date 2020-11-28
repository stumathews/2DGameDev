#pragma once
#include "Event.h"
#include <memory>
#include "game_object.h"

class AddGameObjectToCurrentSceneEvent : public event
{
	std::shared_ptr<game_object> gameObject_;
public:
	AddGameObjectToCurrentSceneEvent(std::shared_ptr<game_object> gameObject) : event(event_type::AddGameObjectToCurrentScene)
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
	string to_str() override;

private:
	std::shared_ptr<game_object> GameObject;
};

inline string AddGameObjectToCurrentSceneEvent::to_str()
{
	return "add_game_object_to_current_scene_event";
}
