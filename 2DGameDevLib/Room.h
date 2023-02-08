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

	Room(const std::string& name, const std::string& type, int number, int x, int y, int width, int height, bool fill = false);

	bool IsWalled(Side wall) const; // IsWalled (zero based). Does this room have a wall errected on specified side.
	bool HasTopWall() const;
	bool HasBottomWall() const;
	bool HasLeftWall() const;
	bool HasRightWall() const;

	void UpdateInnerBounds();
	void SetupWalls();
	void SetSorroundingRooms(int topIndex, int rightIndex, int bottomIndex, int leftIndex);
	void RemoveWall(Side wall);
	void LogWallRemoval(Side wall) const;
	void SetNotWalled(Side wall);
	void SetWalled(Side wall);
	void AddWall(Side wall);
	void RemoveWallZeroBased(Side wall);
	void ShouldRoomFill(bool fillMe = false);
	void DrawWalls(SDL_Renderer* renderer) const;
	static void DrawLine(SDL_Renderer* renderer, const gamelib::Line& line);
	void DrawDiagnostics(SDL_Renderer* renderer);
	void LoadSettings() override;
	void Draw(SDL_Renderer* renderer) override;
	void Update(unsigned long deltaMs) override;

	gamelib::GameObjectType GetGameObjectType() override { return gamelib::GameObjectType::GameDefined; }
	gamelib::ListOfEvents HandleEvent(std::shared_ptr<gamelib::Event> event, unsigned long deltaMs) override;
	gamelib::ListOfEvents& OnPlayerMoved(std::vector<std::shared_ptr<gamelib::Event>>& generatedEvents);
	gamelib::Coordinate<int> GetCenter(int width, int height) const;
	gamelib::Coordinate<int> GetCenter(gamelib::ABCDRectangle rectangle) const;

	gamelib::Coordinate<int> GetPosition();
	int GetNeighbourIndex(Side index) const;
	int GetX() const;
	int GetY() const;
	int GetWidth() const;
	int GetHeight() const;
	int GetRoomNumber() const;
	int GetRowNumber(int maxCols) const;
	int GetColumnNumber(int maxCols) const;

	std::string GetName() override { return "Room"; }

	SDL_Rect InnerBounds{};
	
	bool IsTopWalled{};
	bool IsBottomWalled{};
	bool IsRightWalled{};
	bool IsLeftWalled{};
	
	gamelib::Line TopLine;
	gamelib::Line RightLine;
	gamelib::Line BottomLine;
	gamelib::Line LeftLine;



protected:
	int roomNumber;
	bool walls[4]{};
	gamelib::ABCDRectangle abcd{};
	gamelib::ABCDRectangle& GetABCDRectangle();
	int topRoomIndex;
	int rightRoomIndex;
	int bottomRoomIndex; 
	int leftRoomIndex;
	int width;
	int height;	

private:	
	bool isPlayerWithinRoom = false;
	bool fill = false;
	int innerBoundsOffset;
	bool logWallRemovals;
	bool drawInnerBounds{};
	bool drawHotSpot{};
	bool printDebuggingTextNeighboursOnly{};
	bool printDebuggingText{};	
};



