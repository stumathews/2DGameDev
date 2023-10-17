#pragma once

#include <memory>
#include <vector>
#include <objects/DrawableGameObject.h>
#include "events/Event.h"
#include "Room.h"
#include <ai/FSM.h>
#include <objects/AnimatedSprite.h>
#include "util/Tuple.h"
#include <Hotspot.h>
#include "RoomInfo.h"
#include "movement/IGameMoveStrategy.h"

using ListOfEvents = std::vector<std::shared_ptr<gamelib::Event>>;
using ListOfGameObjects = std::vector<std::weak_ptr<gamelib::GameObject>>;

namespace gamelib
{
	enum class Direction;
}

class Player : public gamelib::DrawableGameObject
{
public:
	Player(const std::string& name, const std::string& type, gamelib::Coordinate<int> position, int width,
	       int height, const std::string
	       & identifier);

	Player(const std::string& name, const std::string& type, const std::shared_ptr<Room>& playerRoom, int playerWidth,
	       int playerHeight,
	       const std::string& identifier);

	Player(const std::string& name, const std::string& type, const std::shared_ptr<Room>& playerRoom,
	       const std::string& identifier);

	void LoadSettings() override;
	std::vector<std::shared_ptr<gamelib::Event>> HandleEvent(std::shared_ptr<gamelib::Event> event,
	                                                         unsigned long deltaMs) override;
	std::string GetName() override { return Name; }
	void Fire() const;
	void OnGameWon();
	void RemovePlayerFacingWall() const;
	void RemoveRightWall() const;
	void RemoveLeftWall() const;
	void RemoveBottomWall() const;
	void RemoveTopWall() const;
	void SetPlayerDirection(gamelib::Direction direction);
	void Update(unsigned long deltaMs) override;
	void Draw(SDL_Renderer* renderer) override;

	void SetSprite(const std::shared_ptr<gamelib::AnimatedSprite>& inSprite);

	void SetMoveStrategy(const std::shared_ptr<gamelib::IGameObjectMoveStrategy>& inMoveStrategy)
	{
		moveStrategy = inMoveStrategy;
	}

	[[nodiscard]] int GetHotSpotLength() const { return hotspotSize; }
	[[nodiscard]] int GetWidth() const { return width; }
	[[nodiscard]] int GetHeight() const { return height; }
	gamelib::GameObjectType GetGameObjectType() override { return gamelib::GameObjectType::GameDefined; }
	int GetHealth();
	int GetPoints();
	std::shared_ptr<RoomInfo> CurrentRoom;
	std::string Identifier;
	std::shared_ptr<gamelib::Hotspot> Hotspot;

private:
	void CommonInit(int playerWidth, int playerHeight, const std::string& identifier);
	void CenterPlayerInRoom(const std::shared_ptr<Room>& targetRoom);
	void BaseProcessEvent(const std::shared_ptr<gamelib::Event>& event, gamelib::ListOfEvents& createdEvents,
	                      unsigned long deltaMs);
	const gamelib::ListOfEvents& OnControllerMove(const std::shared_ptr<gamelib::Event>& event,
	                                              gamelib::ListOfEvents& createdEvents, unsigned long deltaMs);

	int pixelsToMove = 0;
	std::shared_ptr<gamelib::AnimatedSprite> sprite;
	int width{};
	int height{};
	bool drawBounds = false;
	bool hideSprite = false;
	bool drawHotSpot = false;
	int hotspotSize = 0;
	gamelib::Direction currentMovingDirection;
	gamelib::Direction currentFacingDirection;
	std::shared_ptr<gamelib::IGameObjectMoveStrategy> moveStrategy;
	bool verbose{};
	bool gameWon = false;
};
