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
typedef std::vector<std::shared_ptr<gamelib::GameObject>> ListOfGameObjects;

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
		
	Player(gamelib::coordinate<int> position, const int w, const int h, const std::string identifier);
	Player(std::shared_ptr<Room> playerRoom, int playerWidth, int playerHeight, std::string identifier);
	
	/// <summary>
	/// Common initilization routine
	/// </summary>
	void CommonInit(const int playerWidth, const int playerHeight, const std::string inIdentifier);

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
	std::vector<std::shared_ptr<gamelib::Event>> HandleEvent(std::shared_ptr<gamelib::Event> event, unsigned long deltaMs) override;

	/// <summary>
	/// Get Top Room
	/// </summary>
	/// <returns></returns>
	std::shared_ptr<Room> GetTopRoom() { return GetAdjacentRoomTo(GetCurrentRoom(), Side::Top); }

	/// <summary>
	/// Get player's bottom room
	/// </summary>
	/// <returns></returns>
	std::shared_ptr<Room> GetBottomRoom() { return GetAdjacentRoomTo(GetCurrentRoom(), Side::Bottom); }

	/// <summary>
	/// Get player's right room
	/// </summary>
	/// <returns>Right room</returns>
	std::shared_ptr<Room> GetRightRoom() { return GetAdjacentRoomTo(GetCurrentRoom(), Side::Right); }

	/// <summary>
	/// Get player's left room
	/// </summary>
	/// <returns>Room left of player</returns>
	std::shared_ptr<Room> GetLeftRoom() { return GetAdjacentRoomTo(GetCurrentRoom(), Side::Left); }

	/// <summary>
	/// Remove wall in front of the player
	/// </summary>
	void Fire();

	/// <summary>
	/// Remove the wall that is on the side the direction the player is facing
	/// </summary>
	void RemovePlayerFacingWall();

	/// <summary>
	/// Remove wall in room to the right of player's current room
	/// </summary>
	void RemoveRightWall();

	/// <summary>
	/// Remove wall in room to the left of player
	/// </summary>
	void RemoveLeftWall();

	/// <summary>
	/// Remove wall below the player
	/// </summary>
	void RemoveBottomWall();

	/// <summary>
	/// Remove wall above player
	/// </summary>
	void RemoveTopWall();

	/// <summary>
	/// Allow Game Object to process events
	/// </summary>
	/// <param name="event"></param>
	/// <param name="createdEvents"></param>
	void BaseProcessEvent(const std::shared_ptr<gamelib::Event>& event, gamelib::ListOfEvents& createdEvents, unsigned long deltaMs);

	/// <summary>
	/// What to do when the controller is pressed
	/// </summary>
	/// <param name="event"></param>
	/// <param name="createdEvents"></param>
	/// <returns></returns>
	const gamelib::ListOfEvents& OnControllerMove(const std::shared_ptr<gamelib::Event>& event, gamelib::ListOfEvents& createdEvents, unsigned long deltaMs);

	/// <summary>
	/// Sets the players direction
	/// </summary>
	/// <param name="direction"></param>
	void SetPlayerDirection(gamelib::Direction direction);

	/// <summary>
	/// Get player's room
	/// </summary>
	/// <returns></returns>
	const std::shared_ptr<Room> GetCurrentRoom();

	/// <summary>
	/// Get specific room by index
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	const std::shared_ptr<Room> GetRoomByIndex(int index);

	/// <summary>
	/// Get a side of any room
	/// </summary>
	const std::shared_ptr<Room> GetAdjacentRoomTo(std::shared_ptr<Room> currentRoom, Side side);

	/// <summary>
	/// Draw player
	/// </summary>
	/// <param name="renderer"></param>
	void Draw(SDL_Renderer* renderer) override;

	/// <summary>
	/// Get Name
	/// </summary>
	/// <returns></returns>
	std::string GetName() override { return Identifier; }

	/// <summary>
	/// Get Game object type
	/// </summary>
	/// <returns></returns>
	gamelib::GameObjectType GetGameObjectType() override { return gamelib::GameObjectType::Player; }

	/// <summary>
	/// Update player
	/// </summary>
	 void Update(float deltaMs) override;

	/// <summary>
	/// Update sprite
	/// </summary>
	/// <param name="deltaMs"></param>
	void UpdateSprite(float deltaMs);

	/// <summary>
	/// Check if player has pending moves
	/// </summary>
	/// <returns></returns>
	bool PlayerHasPendingMoves();

	/// <summary>
	/// Move the player
	/// </summary>
	/// <param name="deltaMs"></param>
	void Move(float deltaMs);

	/// <summary>
	/// Depending on the player's direction, a diffirent group/set of key frames in the sprite will cycle
	/// </summary>
	void SetSpriteAnimationFrameGroup();

	/// <summary>	
	/// Set if the player is in this room or not	
	/// </summary>
	bool IsWithinRoom(std::shared_ptr<Room> room);

	/// <summary>
	/// Set the player's room
	/// </summary>
	/// <param name="roomIndex"></param>
	void SetPlayerRoom(int roomIndex);

	/// <summary>
	/// Get players width
	/// </summary>
	/// <returns></returns>
	int GetWidth() { return width; }

	/// <summary>
	/// Get player's height
	/// </summary>
	/// <returns></returns>
	int GetHeight() { return height; }

	/// <summary>
	/// Set the sprite of the player
	/// </summary>
	/// <param name="sprite"></param>
	void SetSprite(std::shared_ptr<gamelib::AnimatedSprite> inSprite) { _sprite = inSprite; }

	/// <summary>
	/// Set the movement strategy
	/// </summary>
	/// <param name="moveStrategy"></param>
	void SetMoveStrategy(std::shared_ptr<IPlayerMoveStrategy> inMoveStrategy) { moveStrategy = inMoveStrategy; }

	/// <summary>
	/// Current room
	/// </summary>
	std::shared_ptr<Room> CurrentRoom;

	/// <summary>
	/// All game objects
	/// </summary>
	ListOfGameObjects& GameObjectsPtr;

	/// <summary>
	/// Get Hotspot length
	/// </summary>
	/// <returns></returns>
	int GetHotSpotLength() { return hotspotSize; }

	/// <summary>
	/// Identifer of the player
	/// </summary>
	std::string Identifier;

	/// <summary>
	/// Player's Hotspot
	/// </summary>
	std::shared_ptr<gamelib::Hotspot> Hotspot;

private:
	int pixelsToMove = 0;
	std::shared_ptr<gamelib::AnimatedSprite> _sprite;
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

};

