#pragma once
#include "Level.h"
#include "character/Npc.h"
#include "time/PeriodicTimer.h"

class Level;
class Player;

class Enemy final : public gamelib::Npc, public std::enable_shared_from_this<Enemy>
{
public:
	Enemy(const std::string& name,
	      const std::string& type, gamelib::Coordinate<int> position,
	      bool visible,
	      const std::shared_ptr<Room>& startRoom, std::shared_ptr<gamelib::AnimatedSprite> sprite,
	      gamelib::Direction startingDirection,
	      std::shared_ptr<const Level> level,
	      std::shared_ptr<gamelib::IGameObjectMoveStrategy> enemyMoveStrategy = nullptr);

	void Initialize();

	std::shared_ptr<RoomInfo> CurrentRoom;
	std::shared_ptr<const Level> CurrentLevel;
	std::vector<std::shared_ptr<gamelib::Event>> HandleEvent(std::shared_ptr<gamelib::Event> event,
	                                                         unsigned long deltaMs) override;
	bool Move(const unsigned long deltaMs); // true if moved
	void Update(unsigned long deltaMs) override;
	void LoadSettings() override;
	std::string GetSubscriberName() override { return Name; }
	std::string GetName() override { return Name; }

private:
	void CheckForPlayerCollision();
	bool isValidMove{};
	bool IfMoved(gamelib::Direction direction) const;
	bool IsPlayerInSameAxis(const std::shared_ptr<Player>& player, bool verticalView) const;
	void LookForPlayer();
	bool IsPlayerInLineOfSight(gamelib::Direction lookDirection) const;
	std::function<void(unsigned long deltaMs)> DoLookForPlayer(); // returns a function taking a delta time
	void ConfigureEnemyBehavior();
	std::function<bool()> IfMovedIn(gamelib::Direction direction) const; // returns a function returning a bool	
	bool emitMoveEvents{};
	bool moveAtSpeed {};	
	int speed{};
	gamelib::PeriodicTimer moveTimer;
	int moveRateMs{};
	bool animate = true;
	bool drawState = false;
};
