#pragma once
#include <memory>
#include <events/GameObjectEvent.h>


class GameObjectEventFactory
{
	public:
	static std::shared_ptr<gamelib::GameObjectEvent> MakeRemoveObjectEvent(gamelib::GameObject* target)
	{
		return std::shared_ptr<gamelib::GameObjectEvent>(new gamelib::GameObjectEvent(0, &(*target), gamelib::GameObjectEventContext::Remove));
	}
};

