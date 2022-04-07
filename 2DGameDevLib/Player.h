#pragma once

#include <memory>
#include <vector>

#include <objects/DrawableGameObject.h>
#include "events/Event.h"
#include "Room.h"
#include <ai/FSM.h>

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
	/// Get Index of neighbour
	/// </summary>
	static int GetRoomNeighbourIndex(int firstRoomIndex, int lastRoomIndex, int side, std::shared_ptr<Room> room);

	/// <summary>
	/// Handle Player events
	/// </summary>
	/// <param name="event"></param>
	/// <returns></returns>
	std::vector<std::shared_ptr<gamelib::Event>> HandleEvent(std::shared_ptr<gamelib::Event> event) override;

	void SetDirection(gamelib::Direction direction);

	const ptrdiff_t& CountRoomGameObjects(std::vector<std::shared_ptr<gamelib::GameObject>>& gameObjects);

	const std::shared_ptr<Room>& GetCurrentRoom(std::vector<std::shared_ptr<gamelib::GameObject>>& gameObjects, const int& firstRoomIndex);

	const std::shared_ptr<Room>& GetRoom(std::vector<std::shared_ptr<gamelib::GameObject>>& gameObjects, const int& firstRoomIndex, const ptrdiff_t& lastRoomIndex, std::shared_ptr<Room>& currentRoom, int side);

	bool IsValidMove(const gamelib::Direction& moveDirection, const bool& canMoveDown, const bool& canMoveLeft, const bool& canMoveRight, const bool& canMoveUp);

	bool CanMoveUp(const bool& isMovingUp, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& aboveRoom);

	bool CanMoveDown(const bool& isMovingDown, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& bottomRoom);

	bool CanMoveLeft(const bool& isMovingLeft, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& leftRoom);

	bool CanMoveRight(const bool& isMovingRight, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& rightRoom);

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
	gamelib::object_type GetGameObjectType() override;

	/// <summary>
	/// Update player
	/// </summary>
	void Update() override;

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

	gamelib::Direction GetDirection();

private:
	int width;
	int height;
	int within_room_index = 0;
	bool drawBox = false;
	bool be_verbose;
	gamelib::Direction direction;
	
};

