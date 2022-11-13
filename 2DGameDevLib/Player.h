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
#include "Restrictions.h"
#include <Timer.h>
#include <Movement/Movement.h>
#include <deque>
#include <events/ControllerMoveEvent.h>

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
		
	Player(const int x, const int y, const int w, const int h, const std::string identifier);
	Player(std::shared_ptr<Room> playerRoom, int playerWidth, int playerHeight, std::string identifier);
	
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
	std::vector<std::shared_ptr<gamelib::Event>> HandleEvent(std::shared_ptr<gamelib::Event> event) override;

	std::shared_ptr<Room> GetTopRoom();

	std::shared_ptr<Room> GetBottomRoom();

	std::shared_ptr<Room> GetRightRoom();

	std::shared_ptr<Room> GetLeftRoom();

	void Fire();

	// Remove the wall that is on the side the direction the player is facing
	void RemovePlayerFacingWall();

	void RemoveRightWall();

	const std::shared_ptr<Room> GetRightNeighbourRoom();

	void RemoveLeftWall();

	const std::shared_ptr<Room> GetLeftNeighbourRoom();

	void RemoveBottomWall();

	const std::shared_ptr<Room> GetBottomNeighbourRoom();

	void RemoveTopWall();

	const std::shared_ptr<Room> GetTopNeighbourRoom();

	void BaseProcessEvent(const std::shared_ptr<gamelib::Event>& event, gamelib::ListOfEvents& createdEvents);

	const gamelib::ListOfEvents& OnControllerMove(const std::shared_ptr<gamelib::Event>& event, gamelib::ListOfEvents& createdEvents);

	// Add to player's list of movements to process during its next update cycle
	void AddToPlayerMovements(std::shared_ptr<gamelib::ControllerMoveEvent>& controllerMoveEvent, ListOfEvents& createdEvents);

	
	std::shared_ptr<Room> GetTargettedRoom(std::shared_ptr<gamelib::ControllerMoveEvent> positionChangedEvent, std::shared_ptr<Room> topRoom, std::shared_ptr<Room> bottomRoom, std::shared_ptr<Room> leftRoom, std::shared_ptr<Room> rightRoom);


	// Sets the players direction
	void SetPlayerDirection(gamelib::Direction direction);

	const ptrdiff_t CountRoomGameObjects(ListOfGameObjects& gameObjects);

	const std::shared_ptr<Room> GetCurrentRoom();

	const std::shared_ptr<Room> GetRoom(int index);

	const std::shared_ptr<Room> GetAdjacentRoomTo(std::shared_ptr<Room> currentRoom, Side side);

	gamelib::coordinate<int> GetHotspot();
	

	/// <summary>
	/// Draw player
	/// </summary>
	/// <param name="renderer"></param>
	void Draw(SDL_Renderer* renderer) override;

	void DrawSprite(SDL_Renderer* renderer);

	void DrawBounds(SDL_Renderer* renderer);

	void DrawHotspot(SDL_Renderer* renderer);

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

	void UpdateSprite(float deltaMs);

	bool PlayerHasPendingMoves();

	void Move(float deltaMs);

	// Depending on the player's direction, a diffirent group/set of key frames in the sprite will cycle
	void SetSpriteAnimationFrameGroup();

	// Set if the player is in this room or not		
	bool IsWithinRoom(std::shared_ptr<Room> room);

	SDL_Rect CalculateBounds(int x, int y);

	/// <summary>
	/// Set the player's room
	/// </summary>
	/// <param name="roomIndex"></param>
	void SetPlayerRoom(int roomIndex);

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
	void SetMoveStrategy(std::shared_ptr<IPlayerMoveStrategy> moveStrategy);

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
	ListOfGameObjects& GameObjectsPtr;

	/// <summary>
	/// Calculate player hostspot position
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <returns></returns>
	gamelib::coordinate<int> CalculateHotspotPosition(int x, int y);

	int GetHotSpotLength();

	std::string Identifier;
private:
	int moveDurationMs = 0;
	int maxPixelsToMove = 0;
	std::shared_ptr<gamelib::AnimatedSprite> sprite;
	int width;
	int height;
	int playerRoomIndex = 0;
	bool drawBounds = false;
	bool hideSprite = false;
	bool drawHotSpot = false;
	int hotspotSize = 0;
	gamelib::Direction currentMovingDirection;
	gamelib::Direction currentFacingDirection;
	std::shared_ptr<IPlayerMoveStrategy> moveStrategy;
	std::deque<std::shared_ptr<Movement>> moveQueue;	
	bool debugMovement;
	bool verbose;
};

