#pragma once
#include <SDL.h>
#include <scene/ABCDRectangle.h>
#include "objects/DrawableGameObject.h"
#include "objects/GameObject.h"
#include <events/Event.h>
#include "util/Tuple.h"
#include <geometry/Line.h>
#include <common/aliases.h>

#include "geometry/Side.h"

class Room : public gamelib::DrawableGameObject, public std::enable_shared_from_this<Room>
{
public:
	Room(const std::string& name, const std::string& type, int number, int x, int y, int width, int height,
	     bool fill = false);

	bool IsWalled(gamelib::Side wall) const;
	// IsWalled (zero based). Does this room have a wall errected on specified side.
	bool HasTopWall() const;
	bool HasBottomWall() const;
	bool HasLeftWall() const;
	bool HasRightWall() const;

	void UpdateInnerBounds();
	void SetupWalls();
	void SetSorroundingRooms(int top_index, int right_index, int bottom_index, int left_index,
	                         std::vector<std::shared_ptr<
		                         Room>> rooms);
	void RemoveWall(gamelib::Side wall);
	void LogWallRemoval(gamelib::Side wall) const;
	void SetNotWalled(gamelib::Side wall);
	void SetWalled(gamelib::Side wall);
	void AddWall(gamelib::Side wall);
	void RemoveWallZeroBased(gamelib::Side wall);
	void ShouldRoomFill(bool fillMe = false);
	void DrawWalls(SDL_Renderer* renderer) const;
	std::shared_ptr<Room> GetSideRoom(gamelib::Side side);
	static void DrawLine(SDL_Renderer* renderer, const gamelib::Line& line);
	void DrawDiagnostics(SDL_Renderer* renderer);
	void LoadSettings() override;
	void Draw(SDL_Renderer* renderer) override;
	void Update(unsigned long deltaMs) override;

	gamelib::GameObjectType GetGameObjectType() override { return gamelib::GameObjectType::GameDefined; }
	gamelib::ListOfEvents HandleEvent(std::shared_ptr<gamelib::Event> event, unsigned long deltaMs) override;
	gamelib::ListOfEvents& OnPlayerMoved(std::vector<std::shared_ptr<gamelib::Event>>& generatedEvents);
	gamelib::Coordinate<int> GetCenter(int width, int height) const;
	gamelib::Coordinate<int> GetCenter() const;
	gamelib::Coordinate<int> GetCenter(gamelib::AbcdRectangle rectangle) const;

	gamelib::Coordinate<int> GetPosition();
	int GetNeighbourIndex(gamelib::Side index) const;
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
	gamelib::AbcdRectangle abcd{};
	gamelib::AbcdRectangle& GetABCDRectangle();
	int topRoomIndex;
	std::shared_ptr<Room> RightRoom;
	std::shared_ptr<Room> LeftRoom;
	std::shared_ptr<Room> TopRoom;
	std::shared_ptr<Room> BottomRoom;
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
