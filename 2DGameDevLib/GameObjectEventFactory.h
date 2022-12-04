#pragma once
#include <memory>
#include <events/GameObjectEvent.h>


class GameObjectEventFactory
{
	public:
	static std::shared_ptr<gamelib::GameObjectEvent> MakeRemoveObjectEvent(std::shared_ptr<gamelib::GameObject> target)
	{
		return std::shared_ptr<gamelib::GameObjectEvent>(new gamelib::GameObjectEvent(target, gamelib::GameObjectEventContext::Remove));
	}
};

