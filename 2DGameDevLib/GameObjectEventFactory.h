#pragma once
#include <memory>
#include <events/GameObjectEvent.h>

class GameObjectEventFactory
{
	public:
	static std::shared_ptr<gamelib::GameObjectEvent> MakeRemoveObjectEvent(const std::shared_ptr<gamelib::GameObject>&
		target)
	{
		return std::make_shared<gamelib::GameObjectEvent>(target, gamelib::GameObjectEventContext::Remove);
	}
};

