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
#include <Timer.h>
#include <Movement/Movement.h>
#include <deque>
#include <events/ControllerMoveEvent.h>
#include <Hotspot.h>

typedef std::vector<std::shared_ptr<gamelib::Event>> ListOfEvents;
typedef std::vector<std::weak_ptr<gamelib::GameObject>> ListOfGameObjects;

namespace gamelib { enum class Direction; }

class Player final : public gamelib::DrawableGameObject
{		
public:
		
	Player(std::string name, std::string type, gamelib::coordinate<int> position, const int w, const int h, const std::string identifier);
	Player(std::string name, std::string type, std::shared_ptr<Room> playerRoom, int playerWidth, int playerHeight, std::string identifier);
	
	void LoadSettings() override;
	void CenterPlayerInRoom(std::shared_ptr<Room> target_room);
	std::vector<std::shared_ptr<gamelib::Event>> HandleEvent(std::shared_ptr<gamelib::Event> event, unsigned long deltaMs) override;
	std::shared_ptr<Room> GetTopRoom() { return GetAdjacentRoomTo(GetCurrentRoom(), Side::Top); }
	std::shared_ptr<Room> GetBottomRoom() { return GetAdjacentRoomTo(GetCurrentRoom(), Side::Bottom); }
	std::shared_ptr<Room> GetRightRoom() { return GetAdjacentRoomTo(GetCurrentRoom(), Side::Right); }
	std::shared_ptr<Room> GetLeftRoom() { return GetAdjacentRoomTo(GetCurrentRoom(), Side::Left); }
	const std::shared_ptr<Room> GetCurrentRoom();
	const std::shared_ptr<Room> GetRoomByIndex(int index);
	const std::shared_ptr<Room> GetAdjacentRoomTo(std::shared_ptr<Room> currentRoom, Side side);	
	std::string GetName() override { return Identifier; }
	std::string GetSpriteAnimationFrameGroupForPlayer();
	void Fire();
	void OnGameWon();
	void RemovePlayerFacingWall();
	void RemoveRightWall();
	void RemoveLeftWall();
	void RemoveBottomWall();
	void RemoveTopWall();
	void BaseProcessEvent(const std::shared_ptr<gamelib::Event>& event, gamelib::ListOfEvents& createdEvents, unsigned long deltaMs);
	void SetPlayerDirection(gamelib::Direction direction);	
	void Update(float deltaMs) override;
	void Draw(SDL_Renderer* renderer) override;
	bool PlayerHasPendingMoves();
	bool IsWithinRoom(std::shared_ptr<Room> room);
	void SetPlayerRoom(std::shared_ptr<Room> room);
	void SetSprite(std::shared_ptr<gamelib::AnimatedSprite> inSprite) { sprite = inSprite; }
	void SetMoveStrategy(std::shared_ptr<IPlayerMoveStrategy> inMoveStrategy) { moveStrategy = inMoveStrategy; }
	int GetHotSpotLength() { return hotspotSize; }
	int GetWidth() { return width; }
	int GetHeight() { return height; }
	gamelib::GameObjectType GetGameObjectType() override { return gamelib::GameObjectType::GameDefined; }
	const gamelib::ListOfEvents& OnControllerMove(const std::shared_ptr<gamelib::Event>& event, gamelib::ListOfEvents& createdEvents, unsigned long deltaMs);

	std::shared_ptr<Room> CurrentRoom;	
	std::string Identifier;
	std::shared_ptr<gamelib::Hotspot> Hotspot;

private:
	void commonInit(const int playerWidth, const int playerHeight, const std::string inIdentifier);
	int pixelsToMove = 0;
	std::shared_ptr<gamelib::AnimatedSprite> sprite;
	int width;
	int height;
	int playerRoomIndex = 0;
	bool _drawBounds = false;
	bool hideSprite = false;
	bool drawHotSpot = false;
	int hotspotSize = 0;
	gamelib::Direction currentMovingDirection;
	gamelib::Direction currentFacingDirection;
	std::shared_ptr<IPlayerMoveStrategy> moveStrategy;
	std::deque<std::shared_ptr<gamelib::IMovement>> moveQueue;
	bool _verbose;
	bool gameWon = false;
};

