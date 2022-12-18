#pragma once
#include <SDL.h>
#include <scene/ABCDRectangle.h>
#include "objects/DrawableGameObject.h"
#include "objects/GameObject.h"
#include <events/Event.h>
#include "util/Tuple.h"
#include <geometry/Line.h>
#include <common/aliases.h>
#include "Side.h"
#include <objects/MultipleInheritableEnableSharedFromThis.h>

class Room : public gamelib::DrawableGameObject, public inheritable_enable_shared_from_this<Room>
{	
public:		

	Room(std::string name, std::string type, int number, int x, int y, int width, int height, bool fill = false);

	bool IsWalled(Side wall); // IsWalled (zero based). Does this room have a wall errected on specified side.
	bool HasTopWall();
	bool HasBottomWall();
	bool HasLeftWall();
	bool HasRightWall();

	void UpdateInnerBounds();
	void SetupWalls();
	void SetSorroundingRooms(const int topIndex, const int rightIndex, const int bottomIndex, const int leftIndex);
	void RemoveWall(Side wall);
	void LogWallRemoval(Side wall);
	void SetNotWalled(Side wall);
	void SetWalled(Side wall);
	void AddWall(Side wall);
	void RemoveWallZeroBased(Side wall);
	void ShouldRoomFill(bool fill_me = false);
	void DrawWalls(SDL_Renderer* renderer);
	void DrawLine(SDL_Renderer* renderer, gamelib::Line& line);
	void DrawDiagnostics(SDL_Renderer* renderer);
	void LoadSettings() override;
	void Draw(SDL_Renderer* renderer) override;
	void Update(float deltaMs) override;

	gamelib::GameObjectType GetGameObjectType() override { return gamelib::GameObjectType::GameDefined; }
	gamelib::ListOfEvents HandleEvent(std::shared_ptr<gamelib::Event> event, unsigned long deltaMs) override;
	gamelib::ListOfEvents& OnPlayerMoved(std::vector<std::shared_ptr<gamelib::Event>>& generatedEvents);
	const gamelib::coordinate<int> GetCenter(const int width, const int height);
	
	gamelib::coordinate<int> GetPosition();
	int GetNeighbourIndex(Side index) const;
	int GetX() const;
	int GetY() const;
	int GetWidth() const;
	int GetHeight() const;
	int GetRoomNumber();
	int GetRowNumber(int MaxCols);
	int GetColumnNumber(int MaxCols);

	std::string GetName() override { return "Room"; }

	SDL_Rect InnerBounds;
	
	bool IsTopWalled;
	bool IsBottomWalled;
	bool IsRightWalled;
	bool IsLeftWalled;
	
	gamelib::Line TopLine;
	gamelib::Line RightLine;
	gamelib::Line BottomLine;
	gamelib::Line LeftLine;



protected:
	int RoomNumber;
	bool walls[4];
	gamelib::ABCDRectangle abcd;
	gamelib::ABCDRectangle& GetABCDRectangle();
	int topRoomIndex;
	int rightRoomIndex;
	int bottomRoomIndex; 
	int leftRoomIndex;
	int Width;
	int Height;	

private:	
	bool isPlayerWithinRoom = false;
	bool fill = false;
	int innerBoundsOffset;
	bool logWallRemovals;
	bool drawInnerBounds;
	bool drawHotSpot;
	bool printDebuggingTextNeighboursOnly;
	bool printDebuggingText;	
};



