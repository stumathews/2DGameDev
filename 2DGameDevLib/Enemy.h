#pragma once
#include "Level.h"
#include "objects/Npc.h"

class Level;

class Enemy final : public gamelib::Npc, public std::enable_shared_from_this<Enemy>
{
public:
	auto DoLookForPlayer();
	auto IfMovedIn(gamelib::Direction direction) const;
	Enemy(const std::string& name, const std::string& type, gamelib::Coordinate<int> position, bool visible,
	      const std::shared_ptr<Room>& startRoom, std::shared_ptr<gamelib::AnimatedSprite> sprite, gamelib::Direction startingDirection,
	      std::shared_ptr<const Level> level,
	      std::shared_ptr<gamelib::IGameObjectMoveStrategy> enemyMoveStrategy = nullptr);
	bool IfMoved(gamelib::Direction direction) const;
	void LookForPlayer();
	void DoLookForPlayer(unsigned long deltaMs);
	bool LookForPlayerInDirection(gamelib::Direction lookDirection) const;

	void Initialize();
	void CheckForPlayerCollision();

	std::shared_ptr<RoomInfo> CurrentRoom;
	std::shared_ptr<const Level> CurrentLevel;
	std::vector<std::shared_ptr<gamelib::Event>> HandleEvent(std::shared_ptr<gamelib::Event> event, unsigned long deltaMs) override;
	void Update(unsigned long deltaMs) override;	
	std::string GetSubscriberName() override { return Name;}
	std::string GetName() override { return Name;}
private:
	bool isValidMove{};
};

