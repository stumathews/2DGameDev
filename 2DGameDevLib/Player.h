#pragma once

#include <memory>
#include <vector>
#include <objects/DrawableGameObject.h>
#include "events/Event.h"
#include "Room.h"
#include <ai/FSM.h>
#include <objects/sprite.h>
#include "MoveStrategy.h"
#include "util/Tuple.h"
#include "Restrictions.h"
#include <Timer.h>
#include "Movement.h"
#include <deque>

namespace gamelib
{
	enum class Direction;
}

/// <summary>
/// Player object
/// </summary>

class Player final : public gamelib::DrawableGameObject
{		
public:
		
	Player(int x, int y, int w, int h);
		
	/// <summary>
	/// Load player settings
	/// </summary>
	/// <param name="settings"></param>
	void LoadSettings() override;
		
	/// <summary>
	/// Center player in room
	/// </summary>
	/// <param name="target_room"></param>
	void CenterPlayerInRoom(std::shared_ptr<Room> target_room);

	/// <summary>
	/// Handle Player events
	/// </summary>
	/// <param name="event"></param>
	/// <returns></returns>
	std::vector<std::shared_ptr<gamelib::Event>> HandleEvent(std::shared_ptr<gamelib::Event> event) override;

	void Fire();

	void RemoveWall(gamelib::Direction facaingDirection);

	void RemoveRightWall();

	const std::shared_ptr<Room> GetRightNeighbourRoom();

	void RemoveLeftWall();

	const std::shared_ptr<Room> GetLeftNeighbourRoom();

	void RemoveBottomWall();

	const std::shared_ptr<Room> GetBottomNeighbourRoom();

	void RemoveTopWall();

	const std::shared_ptr<Room> GetTopNeighbourRoom();

	void BaseProcessEvent(const std::shared_ptr<gamelib::Event>& event, gamelib::events& createdEvents);

	const gamelib::events& OnControllerMove(const std::shared_ptr<gamelib::Event>& event, gamelib::events& createdEvents);

	void OnAfterMove(const gamelib::Direction& movementDirection);

	void SetRoomRestrictions();

	void SetMovingDirection(gamelib::Direction direction);

	const ptrdiff_t CountRoomGameObjects(std::vector<std::shared_ptr<gamelib::GameObject>>& gameObjects);

	const std::shared_ptr<Room> GetCurrentRoom(std::vector<std::shared_ptr<gamelib::GameObject>>& gameObjects);
	const std::shared_ptr<Room> GetCurrentRoom();

	const std::shared_ptr<Room> GetRoom(int index);

	const std::shared_ptr<Room> GetRoom(std::vector<std::shared_ptr<gamelib::GameObject>>& gameObjects, std::shared_ptr<Room>& currentRoom, Side side);

	bool IsValidMove(const gamelib::Direction& moveDirection, const bool& canMoveDown, const bool& canMoveLeft, const bool& canMoveRight, const bool& canMoveUp);

	gamelib::coordinate<int> GetHotspot();
	

	/// <summary>
	/// Draw player
	/// </summary>
	/// <param name="renderer"></param>
	void Draw(SDL_Renderer* renderer) override;

	/// <summary>
	/// Get Name
	/// </summary>
	/// <returns></returns>
	std::string GetName() override;

	/// <summary>
	/// Get Game oibject type
	/// </summary>
	/// <returns></returns>
	gamelib::GameObjectType GetGameObjectType() override;

	/// <summary>
	/// Update player
	/// </summary>
	void Update(float deltaMs) override;

	bool IsWithinRoom(std::shared_ptr<Room> room);

	/// <summary>
	/// Set the player's room
	/// </summary>
	/// <param name="roomIndex"></param>
	void SetRoom(int roomIndex);

	/// <summary>
	/// Get players width
	/// </summary>
	/// <returns></returns>
	int GetWidth();

	/// <summary>
	/// Get player's height
	/// </summary>
	/// <returns></returns>
	int GetHeight();

	/// <summary>
	/// Get Direction
	/// </summary>
	/// <returns></returns>
	gamelib::Direction GetDirection();


	/// <summary>
	/// Set the sprite of the player
	/// </summary>
	/// <param name="sprite"></param>
	void SetSprite(std::shared_ptr<gamelib::AnimatedSprite> sprite);

	/// <summary>
	/// Set the movement strategy
	/// </summary>
	/// <param name="moveStrategy"></param>
	void SetMoveStrategy(std::shared_ptr<IMoveStrategy> moveStrategy);

	/// <summary>
	/// Restrictions
	/// </summary>
	Restrictions restrictions;

	/// <summary>
	/// Current room
	/// </summary>
	std::shared_ptr<Room> CurrentRoom;

	/// <summary>
	/// All game objects
	/// </summary>
	std::vector<std::shared_ptr<gamelib::GameObject>>& GameObjectsPtr;
	
	/// <summary>
	/// Animation timeout timer
	/// </summary>
	gamelib::Timer animationTimeoutTimer;

	/// <summary>
	/// Calculate player hostspot position
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <returns></returns>
	gamelib::coordinate<int> CalculateHotspotPosition(int x, int y);
private:
	
	std::shared_ptr<gamelib::AnimatedSprite> sprite;
	int width;
	int height;
	int playerRoomIndex = 0;
	bool drawBox = false;
	gamelib::Direction currentMovingDirection;
	gamelib::Direction currentFacingDirection;
	std::shared_ptr<IMoveStrategy> moveStrategy;
	std::deque<std::shared_ptr<Movement>> moveQueue;
};

