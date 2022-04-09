#pragma once
#include <SDL.h>
#include <scene/ABCDRectangle.h>
#include "objects/DrawableGameObject.h"
#include "objects/GameObject.h"
#include <events/Event.h>

enum class Side
{
	Top,
	Right,
	Bottom,
	Left
};

/// <summary>
/// Conceptual room object
/// </summary>
class Room : public gamelib::DrawableGameObject
{	
public:		
		
	/// <summary>
	/// Create a room
	/// </summary>
	Room(int number, 
		int x,
		int y, 
		int width, 
		int height, 
		bool fill = false);
		
		
	/// <summary>
	/// Get Sorrounding rooms
	/// </summary>
	void SetSoroundingRooms(const int top_index, const int right_index, const int bottom_index,
		                            const int left_index);

	int GetNeighbourIndex(Side index) const;

	/// <summary>
	/// Get X coord
	/// </summary>
	/// <returns></returns>
	int GetX() const;

	/// <summary>
	/// Get Y coord
	/// </summary>
	/// <returns></returns>
	int GetY() const;

	/// <summary>
	/// Get Width
	/// </summary>
	/// <returns></returns>
	int GetWidth() const;

	/// <summary>
	/// Get Heigh
	/// </summary>
	/// <returns></returns>
	int GetHeight() const;

	/// <summary>
	/// IsWalled (zero based)
	/// </summary>
	/// <param name="wall"></param>
	/// <returns></returns>
	bool IsWalled(Side wall);

	/// <summary>
	/// Remove wall
	/// </summary>
	void RemoveWall(Side wall);

	/// <summary>
	/// Add a wall
	/// </summary>
	/// <param name="wall"></param>
	void AddWall(Side wall);

	/// <summary>
	/// Remove wall
	/// </summary>
	void RemoveWallZeroBased(Side wall);
		
	/// <summary>
	/// Should fill room?
	/// </summary>
	void ShouldRoomFill(bool fill_me = false);

	/// <summary>
	/// Provide Room type
	/// </summary>
	gamelib::GameObjectType GetGameObjectType() override;

	/// <summary>
	/// Name
	/// </summary>
	/// <returns></returns>
	std::string GetName() override;
		
	/// <summary>
	/// Handle room events
	/// </summary>
	/// <param name="event"></param>
	/// <returns></returns>
	std::vector<std::shared_ptr<gamelib::Event>> HandleEvent(std::shared_ptr<gamelib::Event> event) override;

	/// <summary>
	/// Load room settings
	/// </summary>
	/// <param name="settings_admin"></param>
	void LoadSettings() override;		 

	/// <summary>
	/// Draw room
	/// </summary>
	/// <param name="renderer"></param>
	void Draw(SDL_Renderer* renderer) override;

	/// <summary>
	/// Update room
	/// </summary>
	void Update() override;

protected:

	/// <summary>
	/// Each room as a room number
	/// </summary>
	int number;

	/// <summary>
	/// Each room as 4 possible walls
	/// </summary>
	bool walls[4];

	/// <summary>
	/// Each room's geometry is managed by a ABCD rectandle
	/// </summary>
	gamelib::ABCDRectangle abcd;

	/// <summary>
	/// Get ABCD Rectangle
	/// </summary>
	/// <returns></returns>
	gamelib::ABCDRectangle& GetABCDRectangle();
		
	/// <summary>
	/// Index of the room above this one (room number)
	/// </summary>
	int topRoomIndex;

	/// <summary>
	/// Index of the room to the right of this one (room number)
	/// </summary>
	int rightRoomIndex;

	/// <summary>
	/// Index of the room below this one (room number)
	/// </summary>
	int bottomRoomIndex; 

	/// <summary>
	/// Room to the left of this one (room number)
	/// </summary>
	int leftRoomIndex;

	/// <summary>
	/// Width of this room
	/// </summary>
	int width;

	/// <summary>
	/// Height of this room
	/// </summary>
	int height;	

private:
	
	/// <summary>
	/// Indication if the player is within this room at the moment
	/// </summary>
	bool isPlayerWithinRoom = false;

	/// <summary>
	/// indication if this room should be filled
	/// </summary>
	bool fill = false;

	/// <summary>
	/// The bounds of the player
	/// </summary>
	SDL_Rect playerBounds = {0};
};



