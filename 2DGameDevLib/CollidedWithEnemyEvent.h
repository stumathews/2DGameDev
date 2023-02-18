#pragma once
#include "Enemy.h"
#include "EventIds.h"
#include "Player.h"
#include "events/Event.h"

class CollidedWithEnemyEvent final : public gamelib::Event
{
public:
	explicit CollidedWithEnemyEvent(std::shared_ptr<Enemy> enemy, std::shared_ptr<Player> other):
		Event(CollidedWithEnemyEventId), Enemy(std::move(enemy)), Other(std::move(other))
	{
	}
	std::shared_ptr<Enemy> Enemy;
	std::shared_ptr<Player> Other;
};
