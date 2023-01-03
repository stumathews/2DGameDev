#pragma once

#include <memory>
#include <vector>
#include <objects/DrawableGameObject.h>
#include "events/Event.h"
#include "Room.h"
#include <ai/FSM.h>
#include <objects/AnimatedSprite.h>
#include "MoveStrategy.h"
#include "util/Tuple.h"
#include <Movement/Movement.h>
#include <deque>
#include <Hotspot.h>

typedef std::vector<std::shared_ptr<gamelib::Event>> ListOfEvents;
typedef std::vector<std::weak_ptr<gamelib::GameObject>> ListOfGameObjects;

namespace gamelib { enum class Direction; }

class Player : public gamelib::DrawableGameObject
{		
public:
		
	Player(const std::string& name, const std::string& type, gamelib::Coordinate<int> position, int width,
	       int height, const std::string
	       & identifier);
	Player(const std::string& name, const std::string& type, const std::shared_ptr<Room>& playerRoom, int playerWidth, int playerHeight,
	       const std::string& identifier);
	
	void LoadSettings() override;
	void CenterPlayerInRoom(const std::shared_ptr<Room>& targetRoom);
	std::vector<std::shared_ptr<gamelib::Event>> HandleEvent(std::shared_ptr<gamelib::Event> event, unsigned long deltaMs) override;
	[[nodiscard]] std::shared_ptr<Room> GetTopRoom() const { return GetAdjacentRoomTo(GetCurrentRoom(), Side::Top); }
	[[nodiscard]] std::shared_ptr<Room> GetBottomRoom() const { return GetAdjacentRoomTo(GetCurrentRoom(), Side::Bottom); }
	[[nodiscard]] std::shared_ptr<Room> GetRightRoom() const { return GetAdjacentRoomTo(GetCurrentRoom(), Side::Right); }
	[[nodiscard]] std::shared_ptr<Room> GetLeftRoom() const { return GetAdjacentRoomTo(GetCurrentRoom(), Side::Left); }
	[[nodiscard]] std::shared_ptr<Room> GetCurrentRoom() const;
	static std::shared_ptr<Room> GetRoomByIndex(int index);
	static std::shared_ptr<Room> GetAdjacentRoomTo(const std::shared_ptr<Room>& currentRoom, Side side);	
	std::string GetName() override { return Identifier; }
	[[nodiscard]] std::string GetSpriteAnimationFrameGroupForPlayer() const;
	void Fire();
	void OnGameWon();
	void RemovePlayerFacingWall();
	void RemoveRightWall() const;
	void RemoveLeftWall() const;
	void RemoveBottomWall() const;
	void RemoveTopWall() const;
	void BaseProcessEvent(const std::shared_ptr<gamelib::Event>& event, gamelib::ListOfEvents& createdEvents, unsigned long deltaMs);
	void SetPlayerDirection(gamelib::Direction direction);	
	void Update(float deltaMs) override;
	void Draw(SDL_Renderer* renderer) override;
	[[nodiscard]] bool PlayerHasPendingMoves() const;
	[[nodiscard]] bool IsWithinRoom(const std::shared_ptr<Room>& room) const;
	void SetPlayerRoom(const std::shared_ptr<Room>& room);
	void SetSprite(const std::shared_ptr<gamelib::AnimatedSprite>& inSprite) { sprite = inSprite; }
	void SetMoveStrategy(const std::shared_ptr<IPlayerMoveStrategy>& inMoveStrategy) { moveStrategy = inMoveStrategy; }
	[[nodiscard]] int GetHotSpotLength() const { return hotspotSize; }
	[[nodiscard]] int GetWidth() const { return width; }
	[[nodiscard]] int GetHeight() const { return height; }
	gamelib::GameObjectType GetGameObjectType() override { return gamelib::GameObjectType::GameDefined; }
	const gamelib::ListOfEvents& OnControllerMove(const std::shared_ptr<gamelib::Event>& event, gamelib::ListOfEvents& createdEvents, unsigned long deltaMs);

	std::shared_ptr<Room> CurrentRoom;	
	std::string Identifier;
	std::shared_ptr<gamelib::Hotspot> Hotspot;

private:
	void commonInit(int playerWidth, int playerHeight, const std::string& inIdentifier);
	int pixelsToMove = 0;
	std::shared_ptr<gamelib::AnimatedSprite> sprite;
	int width{};
	int height{};
	int playerRoomIndex = 0;
	bool drawBounds = false;
	bool hideSprite = false;
	bool drawHotSpot = false;
	int hotspotSize = 0;
	gamelib::Direction currentMovingDirection;
	gamelib::Direction currentFacingDirection;
	std::shared_ptr<IPlayerMoveStrategy> moveStrategy;
	std::deque<std::shared_ptr<gamelib::IMovement>> moveQueue;
	bool verbose{};
	bool gameWon = false;
};

