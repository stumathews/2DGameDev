#include "Room.h"
#include "pch.h"
#include "Room.h"
#include "util/RectDebugging.h"
#include <events/PlayerMovedEvent.h>
#include <scene/SceneManager.h>
#include "Player.h"
#include <sstream>
#include "SideUtils.h"
#include "GameData.h"

using namespace std;
using namespace gamelib;

Room::Room(string name, string type, int number, int x, int y, int width, int height, bool fill) 
	: DrawableGameObject(name, type, coordinate<int>(x, y), true)
{
	this->Bounds = { x, y, width, height };	
	this->Width = width;
	this->Height = height;
	this->RoomNumber = number;
	this->logWallRemovals = false;
	this->fill = fill;
	this->topRoomIndex = 0;
	this->rightRoomIndex = 0;
	this->bottomRoomIndex = 0;
	this->Width = width;
	this->Height = height; 
	this->leftRoomIndex = 0;
	this->innerBoundsOffset = 0;	 
	this->abcd = ABCDRectangle(x, y, width, height);
	UpdateInnerBounds(); // We only need to update the bounds once, so do it in the constructor only
	SetupWalls();
}

void Room::UpdateInnerBounds()
{
	this->InnerBounds = SDL_Rect 
	{ 
		Bounds.x + innerBoundsOffset, 
		Bounds.y + innerBoundsOffset,
		Bounds.w - innerBoundsOffset * 2,
		Bounds.h - innerBoundsOffset * 2
	};
}

void Room::SetupWalls()
{
	/*
		A(ax,ay)----B(bx,by)
		|                  |
		|                  |
		D(dx,dy)----C(cx,cy)
	*/

	// Calculate the geometry of the walls
	auto& rect = this->abcd;
	const auto ax = rect.GetAx(); const auto ay = rect.GetAy();
	const auto bx = rect.GetBx(); const auto by = rect.GetBy();
	const auto cx = rect.GetCx(); const auto cy = rect.GetCy();
	const auto dx = rect.GetDx(); const auto dy = rect.GetDy();

	// Top wall line geometry
	TopLine.x1 = ax; TopLine.y1 = ay;
	TopLine.x2 = bx; TopLine.y2 = by;

	// Right wall line geometry
	RightLine.x1 = bx; RightLine.y1 = by;
	RightLine.x2 = cx; RightLine.y2 = cy;

	// Bottom wall line geometry
	BottomLine.x1 = cx; BottomLine.y1 = cy;
	BottomLine.x2 = dx; BottomLine.y2 = dy;

	// Left wall line geometry
	LeftLine.x1 = dx; LeftLine.y1 = dy;
	LeftLine.x2 = ax; LeftLine.y2 = ay;

	// All walls are present by default
	walls[0] = walls[1] = walls[2] = walls[3] = IsLeftWalled = IsTopWalled = IsRightWalled = IsBottomWalled = true;
}

gamelib::ListOfEvents Room::HandleEvent(const std::shared_ptr<Event> event, unsigned long deltaMs)
{	
	auto generatedEvents(GameObject::HandleEvent(event, deltaMs));

	switch(event->type)
	{
		case EventType::PlayerMovedEventType: { OnPlayerMoved(generatedEvents); }
		break;
		case EventType::SettingsReloaded: { LoadSettings(); }
		break;
		default:
		{
			std::stringstream message("Unhandled subscribed event in Room class:");
			message << event->ToString();
			gamelib::Logger::Get()->LogThis(message.str());
		}
	}
			
	return generatedEvents;
}

gamelib::ListOfEvents& Room::OnPlayerMoved(vector<shared_ptr<Event>>& generatedEvents)
{
	const auto player = dynamic_pointer_cast<Player>(GameData::Get()->player.lock());
	auto playerHotSpotBounds = player->Hotspot->GetBounds();
	SDL_Rect result;
	isPlayerWithinRoom = SDL_IntersectRect(&InnerBounds, &playerHotSpotBounds, &result);
	if(isPlayerWithinRoom) { player->SetPlayerRoom(shared_from_this()); }

	return generatedEvents;
}

void Room::DrawWalls(SDL_Renderer* renderer)
{
	if (HasTopWall()) { DrawLine(renderer, TopLine); }		
	if (HasRightWall()) { DrawLine(renderer, RightLine); }			
	if (HasBottomWall()) { DrawLine(renderer, BottomLine); }		
	if (HasLeftWall()) { DrawLine(renderer, LeftLine); }
}

void Room::DrawLine(SDL_Renderer* renderer, gamelib::Line& line) { SDL_RenderDrawLine(renderer, line.x1, line.y1, line.x2, line.y2); }

void Room::DrawDiagnostics(SDL_Renderer* renderer)
{
	const SDL_Color Red = { 255, 0, 0, 0 };
	const SDL_Color Yellow = { 255, 255, 0, 0 };
	if (fill) { DrawFilledRect(renderer, &Bounds, { 255, 0 ,0 ,0 }); }
		
	if(printDebuggingText)
	{
		const auto player = GameData::Get()->GetPlayer();

		if(printDebuggingTextNeighboursOnly)
		{			
			auto playerRoom = player->GetCurrentRoom();
			if(RoomNumber == playerRoom->topRoomIndex || RoomNumber == playerRoom->rightRoomIndex || 
			   RoomNumber == playerRoom->bottomRoomIndex || RoomNumber == playerRoom->leftRoomIndex)
			{
				RectDebugging::printInRect(renderer, GetTag(), &Bounds, Yellow);
			}

			if(RoomNumber == player->CurrentRoom->RoomNumber) { RectDebugging::printInRect(renderer, GetTag(), &Bounds, Red); }			
		}
		else { RectDebugging::printInRect(renderer, GetTag(), &Bounds, Yellow); }
	}
	
	if(drawHotSpot)
	{
		SDL_Rect point_bounds = { GetPosition().GetX() - Width/2, GetPosition().GetY() +Height/2 };
		SDL_Color Cyan = { 0, 255, 255, 0 };
		DrawFilledRect(renderer, &point_bounds , Cyan);
	}

	if(drawInnerBounds)
	{
		SDL_SetRenderDrawColor(renderer, Yellow.r, Yellow.g, Yellow.b, Yellow.a);
		SDL_RenderDrawRect(renderer, &InnerBounds);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	}
}

void Room::Draw(SDL_Renderer* renderer)
{
	GameObject::Draw(renderer);	
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); // Black
	DrawWalls(renderer);
	DrawDiagnostics(renderer);	
}

void Room::LoadSettings()
{
	GameObject::LoadSettings();	

	fill = SettingsManager::Get()->GetBool("room_fill", "enable");
	innerBoundsOffset = SettingsManager::Get()->GetInt("room", "innerBoundsOffset");	
	logWallRemovals = SettingsManager::Get()->GetBool("room", "logWallRemovals");
	drawInnerBounds = SettingsManager::Get()->GetBool("room", "drawinnerBounds");
	drawHotSpot = SettingsManager::Get()->GetBool("room", "drawHotSpot");
	printDebuggingTextNeighboursOnly = SettingsManager::Get()->GetBool("global", "print_debugging_text_neighbours_only");
	printDebuggingText = SettingsManager::Get()->GetBool("global", "print_debugging_text");
	
	UpdateInnerBounds();
}

int Room::GetX() const { return this->Position.GetX(); }
int Room::GetY() const { return this->Position.GetY(); }

int Room::GetWidth() const { return Width; }
int Room::GetHeight() const { return Height; }

bool Room::IsWalled(Side wall) { return walls[(int)wall]; }

bool Room::HasTopWall() { return IsWalled(Side::Top); }
bool Room::HasBottomWall() { return IsWalled(Side::Bottom); }
bool Room::HasLeftWall() { return IsWalled(Side::Left); }
bool Room::HasRightWall() { return IsWalled(Side::Right); }

void Room::Update(float deltaMs) { }

ABCDRectangle& Room::GetABCDRectangle() { return abcd; }
gamelib::coordinate<int> Room::GetPosition() { return GetABCDRectangle().GetCenter(); }
int Room::GetRoomNumber() { return RoomNumber; }
int Room::GetRowNumber(int MaxCols) { return GetRoomNumber() / MaxCols; }

void Room::AddWall(Side wall) { this->walls[(int)wall] = true; 	SetWalled(wall); }
void Room::RemoveWallZeroBased(Side wall) { this->walls[(int)wall] = false; SetNotWalled(wall); }
void Room::ShouldRoomFill(bool fill_me) { fill = fill_me; }

int Room::GetColumnNumber(int MaxCols)
{
	auto row = GetRowNumber(MaxCols); // row for this roomNumber
	auto rowCol0 = row * MaxCols; // column 0 in this row
	auto col = GetRoomNumber() - rowCol0; // col for this roomNumber
	return col;
}

void Room::SetSorroundingRooms(const int top_index, const int right_index, const int bottom_index, const int left_index)
{
	this->topRoomIndex = top_index;
	this->rightRoomIndex = right_index;
	this->bottomRoomIndex = bottom_index;
	this->leftRoomIndex = left_index;
}

const coordinate<int> Room::GetCenter(const int w, const int h)
{
	auto const room_x_mid = GetX() + (GetWidth() / 2);
	auto const room_y_mid = GetY() + (GetHeight() / 2);
	auto const x = room_x_mid - w /2;
	auto const y = room_y_mid - h /2;			
	return coordinate<int>(x, y);
}

int Room::GetNeighbourIndex(Side side) const
{
	switch (side)
	{
	case Side::Top:
		return topRoomIndex;
		break;
	case Side::Right:
		return rightRoomIndex;
		break;
	case Side::Bottom:
		return bottomRoomIndex;
		break;
		case Side::Left:
		return leftRoomIndex;
		break;
	default:
		return -1;
	}
}

void Room::RemoveWall(Side wall)
{	
	if (walls)
	{
		walls[(int)wall] = false;
		SetNotWalled(wall);
		LogWallRemoval(wall);
	}
}

void Room::LogWallRemoval(Side wall)
{
	if (logWallRemovals)
	{
		std::stringstream message;
		message << "Removed " << SideUtils::SideToString(wall) << " wall in room number " << GetRoomNumber();
		gamelib::Logger::Get()->LogThis(message.str());
	}
}

void Room::SetNotWalled(Side wall)
{
	switch (wall)
	{
	case Side::Top:
		IsTopWalled = false;
		break;
	case Side::Bottom:
		IsBottomWalled = false;
		break;
	case Side::Left:
		IsBottomWalled = false;
		break;
	case Side::Right:
		IsRightWalled = false;
		break;
	}
}

void Room::SetWalled(Side wall)
{
	switch (wall)
	{
		case Side::Top: {IsTopWalled = true; } 	break;
		case Side::Bottom: {IsBottomWalled = true; } break;
		case Side::Left: {IsBottomWalled = true; } break;
		case Side::Right: {IsRightWalled = true; } break;
	}
}



