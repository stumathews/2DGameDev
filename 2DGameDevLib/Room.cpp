#include "Room.h"
#include "pch.h"
#include "Room.h"
#include "util/RectDebugging.h"
#include <events/PlayerMovedEvent.h>
#include <scene/SceneManager.h>
#include "Player.h"
#include <sstream>

using namespace std;
using namespace gamelib;

Room::Room(int number, int x, int y, int width, int height, bool fill) : DrawableGameObject(x, y, true),
	fill(fill),
	topRoomIndex(0), 
	rightRoomIndex(0),  
	bottomRoomIndex(0), 
	Width(width), 
	Height(height), 
	leftRoomIndex(0), 
	innerBoundsOffset(0)
{
	// Bounds of this room
	this->Bounds = { x, y, width, height };
	this->InnerBounds = { Bounds.x + innerBoundsOffset, Bounds.y + innerBoundsOffset, Bounds.w - innerBoundsOffset, Bounds.h - innerBoundsOffset };
	this->Width = width;
	this->Height = height;
	this->RoomNumber = number;

	// Room geometry helper
	this->abcd = ABCDRectangle(x, y, width, height);

	// Calculate the geometry of the walls
	auto& rect = this->abcd;
	const auto ax = rect.GetAx();
	const auto ay = rect.GetAy();
	const auto bx = rect.GetBx();
	const auto by = rect.GetBy();
	const auto cx = rect.GetCx();
	const auto cy = rect.GetCy();
	const auto dx = rect.GetDx();
	const auto dy = rect.GetDy();

	// Top wall line geometry
	TopLine.x1 = ax;
	TopLine.y1 = ay;
	TopLine.x2 = bx;
	TopLine.y2 = by;

	// Right wall line geometry
	RightLine.x1 = bx;
	RightLine.y1 = by;
	RightLine.x2 = cx;
	RightLine.y2 = cy;

	// Bottom wall line geometry
	BottomLine.x1 = cx;
	BottomLine.y1 = cy;
	BottomLine.x2 = dx;
	BottomLine.y2 = dy;

	// Left wall line geometry
	LeftLine.x1 = dx;
	LeftLine.y1 = dy;
	LeftLine.x2 = ax;
	LeftLine.y2 = ay;

	// All walls are present by default
	walls[0] = IsTopWalled = true;
	walls[1] = IsRightWalled = true;
	walls[2] = IsBottomWalled = true;
	walls[3] = IsLeftWalled = true;	 
}

vector<shared_ptr<Event>> Room::HandleEvent(const std::shared_ptr<Event> event)
{	
	auto generatedEvents(GameObject::HandleEvent(event));

	switch(event->type)
	{
	case EventType::PlayerMovedEventType:
		OnPlayerMoved(generatedEvents);
		break;
	case EventType::SettingsReloaded:
		LoadSettings();	
		break;
	default:
		std::stringstream message("Unhandled subscribed event in Room class:");
			message << event->ToString();
		gamelib::Logger::Get()->LogThis(message.str());
	}
			
	return generatedEvents;
}

vector<shared_ptr<Event>>& Room::OnPlayerMoved(vector<shared_ptr<Event>>& generatedEvents)
{
	if(GetGameObjectType() != GameObjectType::Room)
	{
		return generatedEvents;
	}

	const auto player = dynamic_pointer_cast<Player>(gamelib::SceneManager::Get()->GetGameWorld().player);
		
	// Set if the player is in this room or not		
	auto x1 = player->GetHotspot().GetX();
	auto y1 = player->GetHotspot().GetY();
	isPlayerWithinRoom = SDL_IntersectRectAndLine(&InnerBounds, &x1, &y1, &x1, &y1);
			
	// Tell the player that its in this room
	if(isPlayerWithinRoom)
	{
		player->SetRoom(RoomNumber);
	}
	return generatedEvents;
}


void Room::DrawWalls(SDL_Renderer* renderer)
{
	// Keep track of walls
	const auto hasTopWall = this->walls[0];
	const auto hasRightWall = this->walls[1];
	const auto hasBottomWall = this->walls[2];
	const auto hasLeftWall = this->walls[3];

	// Draw the walls as lines
	if (hasTopWall)
	{
		SDL_RenderDrawLine(renderer, TopLine.x1, TopLine.y1, TopLine.x2, TopLine.y2);
	}
		
	if (hasRightWall)
	{
		SDL_RenderDrawLine(renderer, RightLine.x1, RightLine.y1, RightLine.x2, RightLine.y2);
	}

		
	if (hasBottomWall)
	{
		SDL_RenderDrawLine(renderer, BottomLine.x1, BottomLine.y1, BottomLine.x2, BottomLine.y2);
	}
		
	if (hasLeftWall)
	{
		SDL_RenderDrawLine(renderer, LeftLine.x1, LeftLine.y1, LeftLine.x2, LeftLine.y2);
	}
}

void Room::DrawDiagnostics(SDL_Renderer* renderer)
{
	if (fill)
	{
		DrawFilledRect(renderer, &Bounds, { 255, 0 ,0 ,0 });
	}
		
	if(SettingsManager::Get()->GetBool("global", "print_debugging_text"))
	{
		const auto player = dynamic_pointer_cast<Player>(gamelib::SceneManager::Get()->GetGameWorld().player);
		const SDL_Color Red = { 255, 0, 0, 0};
		const SDL_Color Yellow = { 255, 255, 0, 0};

		if(SettingsManager::Get()->GetBool("global", "print_debugging_text_neighbours_only"))
		{			
			auto playerRoom = player->GetCurrentRoom();
			if(RoomNumber == playerRoom->topRoomIndex || RoomNumber == playerRoom->rightRoomIndex || 
			   RoomNumber == playerRoom->bottomRoomIndex || RoomNumber == playerRoom->leftRoomIndex)
			{
				RectDebugging::printInRect(renderer, GetTag(), &Bounds, Yellow);
			}

			if(RoomNumber == player->CurrentRoom->RoomNumber)
			{
				RectDebugging::printInRect(renderer, GetTag(), &Bounds, Red);
			}			
		}
		else
		{			
			RectDebugging::printInRect(renderer, GetTag(), &Bounds, Yellow);
		}
	}
	
	if(SettingsManager::Get()->GetBool("room","drawHotSpot"))
	{
		SDL_Rect point_bounds = { GetHotspot().GetX() - Width/2, GetHotspot().GetY() +Height/2 };
		SDL_Color Cyan = { 0, 255, 255, 0 };
		DrawFilledRect(renderer, &point_bounds , Cyan);
	}

	if(SettingsManager::Get()->GetBool("room","drawinnerBounds"))
	{
		SDL_SetRenderDrawColor(renderer, 255, 255 ,0, 0); // Yellow
		auto innerBounds = SDL_Rect { Bounds.x + innerBoundsOffset, Bounds.y + innerBoundsOffset, Bounds.w - innerBoundsOffset, Bounds.h - innerBoundsOffset };
		SDL_RenderDrawRect(renderer, &innerBounds);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	}
}

void Room::Draw(SDL_Renderer* renderer)
{
	// Allow base class to perform any common drawing operations
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
}

ABCDRectangle& Room::GetABCDRectangle()
{
	return abcd;
}

gamelib::coordinate<int> Room::GetHotspot()
{
	return GetABCDRectangle().GetCenter();
}

int Room::GetRoomNumber()
{
	return RoomNumber;
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

int Room::GetX() const
{
	return this->x;
}

int Room::GetY() const
{
	return this->y;
}

int Room::GetWidth() const
{
	return Width;
}

int Room::GetHeight() const
{
	return Height;
}

bool Room::IsWalled(Side wall)
{
	return walls[(int)wall];
}

bool Room::HasTopWall()
{
	return IsWalled(Side::Top);
}

bool Room::HasBottomWall()
{
	return IsWalled(Side::Bottom);
}

bool Room::HasLeftWall()
{
	return IsWalled(Side::Left);
}

bool Room::HasRightWall()
{
	return IsWalled(Side::Right);
}

void Room::Update(float deltaMs)
{
}

void Room::RemoveWall(Side wall)
{
	this->walls[(int)wall] = false;
	SetNotWalled(wall);
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

void Room::AddWall(Side wall)
{
	this->walls[(int)wall] = true;
	SetWalled(wall);
}

void Room::SetWalled(Side wall)
{
	switch (wall)
	{
	case Side::Top:
		IsTopWalled = true;
		break;
	case Side::Bottom:
		IsBottomWalled = true;
		break;
	case Side::Left:
		IsBottomWalled = true;
		break;
	case Side::Right:
		IsRightWalled = true;
		break;
	}
}

void Room::RemoveWallZeroBased(Side wall)
{
	this->walls[(int)wall] = false;
	SetNotWalled(wall);
}

void Room::ShouldRoomFill(bool fill_me) 
{
	fill = fill_me; 
}

GameObjectType Room::GetGameObjectType() 
{
	return GameObjectType::Room;
}

string Room::GetName()
{
	return "Room";
}
