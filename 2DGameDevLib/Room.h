#pragma once
#include <SDL.h>
#include <scene/ABCDRectangle.h>
#include "objects/DrawableGameObject.h"
#include "objects/GameObject.h"
#include <events/Event.h>
#include "util/Tuple.h"
#include <geometry/Line.h>
#include <common/aliases.h>

enum class Side
{
	Top,
	Right,
	Bottom,
	Left
};

inline const char* ToString(Side side)
{
	switch (side)
	{
		case Side::Top:   return "Top";
		case Side::Right: return "Right";
		case Side::Bottom: return "Bottom";
		case Side::Left: return "Left";
		default:      
			return "[Unknown Side]";
	}
}

/// <summary>
/// Conceptual room object
/// </summary>
class Room : public gamelib::DrawableGameObject
{	
public:		
		
	/// <summary>
	/// Create a room
	/// </summary>
	Room(int number, int x, int y, int width, int height, bool fill = false);

	void CalculateInnerBounds();

	void SetupWalls();
				
	/// <summary>
	/// Get Sorrounding rooms
	/// </summary>
	void SetSorroundingRooms(const int top_index, const int right_index, const int bottom_index, const int left_index);

	/// <summary>
	/// Get the center coordinates of the room
	/// </summary>
	const gamelib::coordinate<int> GetCenter(const int w, const int h);

	/// <summary>
	/// Get Neighbour on side index
	/// </summary>
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

	bool HasTopWall();
	bool HasBottomWall();
	bool HasLeftWall();
	bool HasRightWall();

	/// <summary>
	/// Remove wall
	/// </summary>
	void RemoveWall(Side wall);

	void LogWallRemoval(Side wall);

	void SetNotWalled(Side wall);

	/// <summary>
	/// Add a wall
	/// </summary>
	/// <param name="wall"></param>
	void AddWall(Side wall);

	void SetWalled(Side wall);

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
	gamelib::ListOfEvents HandleEvent(std::shared_ptr<gamelib::Event> event) override;

	/// <summary>
	/// When a player moves...
	/// </summary>
	gamelib::ListOfEvents& OnPlayerMoved(std::vector<std::shared_ptr<gamelib::Event>>& generatedEvents);

	void DrawWalls(SDL_Renderer* renderer);

	void DrawLine(SDL_Renderer* renderer, gamelib::Line& line);

	void DrawDiagnostics(SDL_Renderer* renderer);

	/// <summary>
	/// Load room settings
	/// </summary>
	void LoadSettings() override;		 

	/// <summary>
	/// Draw room
	/// </summary>
	/// <param name="renderer"></param>
	void Draw(SDL_Renderer* renderer) override;

	/// <summary>
	/// Update room
	/// </summary>
	void Update(float deltaMs) override;
	
	/// <summary>
	/// Get coordinates of the room's hotspot
	/// </summary>
	/// <returns></returns>
	gamelib::coordinate<int> GetHotspot();

	/// <summary>
	/// Get Room Number
	/// </summary>
	/// <returns></returns>
	int GetRoomNumber();

	/// <summary>
	/// Area just within the bounds
	/// </summary>
	SDL_Rect InnerBounds;
	
	bool IsTopWalled;
	bool IsBottomWalled;
	bool IsRightWalled;
	bool IsLeftWalled;
	
	gamelib::Line TopLine;
	gamelib::Line RightLine;
	gamelib::Line BottomLine;
	gamelib::Line LeftLine;

	int GetRowNumber(int MaxCols);

	int GetColumnNumber(int MaxCols);

protected:

	/// <summary>
	/// Each room as a room number
	/// </summary>
	int RoomNumber;

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
	int Width;

	/// <summary>
	/// Height of this room
	/// </summary>
	int Height;	


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
	/// Offset from the bounds to make an innerBounds
	/// </summary>
	int innerBoundsOffset;

	/// <summary>
	/// Add log entries when walls are removed
	/// </summary>
	bool logWallRemovals;

	bool drawInnerBounds;
	bool drawHotSpot;
	bool printDebuggingTextNeighboursOnly;
	bool printDebuggingText;
	
};



