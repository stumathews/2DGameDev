#include "pch.h"
#include "Room.h"
#include "util/RectDebugging.h"
#include <events/PlayerMovedEvent.h>
#include <scene/SceneManager.h>
#include "Player.h"

using namespace std;
using namespace gamelib;
/// <summary>
/// Handle Room events
/// </summary>
/// <param name="event"></param>
/// <returns></returns>
vector<shared_ptr<Event>> Room::HandleEvent(const std::shared_ptr<Event> event)
{	
	auto generatedEvents(GameObject::HandleEvent(event));

	// Handle when the player moves
	if(event->type == EventType::PlayerMovedEventType)
	{			
		const auto playerMovedEvent = std::static_pointer_cast<PlayerMovedEvent>(event);
				
		// Get player
		const auto player = dynamic_pointer_cast<Player>(gamelib::SceneManager::Get()->GetGameWorld().player);

		// Dont handle player events
		if(GetGameObjectType() == GameObjectType::Player)
		{
			return generatedEvents;
		}

		// Set if the player is in this room or not
		isPlayerWithinRoom = (player->x >= bounds.x && player->x < bounds.x + bounds.w) && 
				            (player->y >= bounds.y && player->y < bounds.y + bounds.h);

		auto x1 = player->GetHotspot().GetX();
		auto y1 = player->GetHotspot().GetY();
		isPlayerWithinRoom = SDL_IntersectRectAndLine(&InnerBounds, &x1, &y1, &x1, &y1);
			
		// A room lets the player know which room it is in
		// Update the player's knowledge of which room its in
		if(isPlayerWithinRoom)
		{
			player->SetRoom(number);
		}

		// Update square's knowledge of player's bounds for layer use
		playerBounds = 
		{
			player->x, 
			player->y, 
			player->GetWidth(), 
			player->GetHeight()  
		};
	}

	// If settings are reloaded, reload the room settings
	if(event->type == EventType::SettingsReloaded)
	{
		LoadSettings();			
	}
		
	return generatedEvents;
}

/// <summary>
/// Reload room settings
/// </summary>
/// <param name="settings_admin"></param>
void Room::LoadSettings()
{
	// Load game object settings
	GameObject::LoadSettings();	

	// Refetch Room settings
	fill = SettingsManager::Get()->GetBool("room_fill", "enable");

	innerBoundsOffset = SettingsManager::Get()->GetInt("room", "innerBoundsOffset");
}

/// <summary>
/// Draw the room
/// </summary>
/// <param name="renderer"></param>
void Room::Draw(SDL_Renderer* renderer)
{
	// Allow base class to perform any common drawing operations
	GameObject::Draw(renderer);

	// black
	SDL_SetRenderDrawColor(renderer, 0, 0,0,0);

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
		
	if (fill)
	{
		DrawFilledRect(renderer, &bounds, { 255, 0 ,0 ,0 });
	}
		
	if(SettingsManager::Get()->GetBool("global", "print_debugging_text"))
	{
		if(SettingsManager::Get()->GetBool("global", "print_debugging_text_neighbours_only"))
		{
			const auto player = dynamic_pointer_cast<Player>(gamelib::SceneManager::Get()->GetGameWorld().player);
			auto playerRoom = player->GetCurrentRoom();
			if(number == playerRoom->topRoomIndex || 
				number == playerRoom->rightRoomIndex || 
				number == playerRoom->bottomRoomIndex || 
				number == playerRoom->leftRoomIndex)
			{
				RectDebugging::printInRect(renderer, GetTag(), &bounds);
			}
		}
		else
		{
			RectDebugging::printInRect(renderer, GetTag(), &bounds);
		}
	}

	// Draw hotspot
	if(SettingsManager::Get()->GetBool("room","drawHotSpot"))
	{
		SDL_Rect point_bounds = { GetHotspot().GetX() - width/2, GetHotspot().GetY() +height/2 };
		DrawFilledRect(renderer, &point_bounds , { 0, 255 , 255 ,0 }); // cyan
	}

	// Draw inner bounds
	if(SettingsManager::Get()->GetBool("room","drawinnerBounds"))
	{
		SDL_SetRenderDrawColor(renderer, 255, 255 , 0,0); //yellow
		auto innerBounds = SDL_Rect { bounds.x + innerBoundsOffset, bounds.y + innerBoundsOffset, bounds.w - innerBoundsOffset, bounds.h - innerBoundsOffset };
		SDL_RenderDrawRect(renderer, &innerBounds);
		SDL_SetRenderDrawColor(renderer, 0, 0,0,0);
	}
}

/// <summary>
/// Get room's  ABCD rectangle
/// </summary>
/// <returns>The Rooms ABCD rectangle</returns>
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
	return number;
}

Room::Room(int number, 
	int x, 
	int y, 
	int width, 
	int height, 
	bool fill)
	: DrawableGameObject(x, y, true), fill(fill), playerBounds({}),
		topRoomIndex(0), rightRoomIndex(0),  bottomRoomIndex(0), width(width), height(height), leftRoomIndex(0), innerBoundsOffset(0)
{
	// Bounds of this room
	this->bounds = { x, y, width, height };
	this->InnerBounds = { bounds.x + innerBoundsOffset, bounds.y + innerBoundsOffset, bounds.w - innerBoundsOffset, bounds.h - innerBoundsOffset };
	this->width = width;
	this->height = height;
	this->number = number;

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

	// Left wall line geometry , , , 
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

/// <summary>
/// Set Indexs of sorrounding rooms
/// </summary>
void Room::SetSoroundingRooms(const int top_index, const int right_index, const int bottom_index, const int left_index)
{
	this->topRoomIndex = top_index;
	this->rightRoomIndex = right_index;
	this->bottomRoomIndex = bottom_index;
	this->leftRoomIndex = left_index;
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

/// <summary>
/// corner x-coordinate
/// </summary>
/// <returns></returns>
int Room::GetX() const
{
	return this->x;
}

/// <summary>
/// cornder y-coorinate
/// </summary>
/// <returns></returns>
int Room::GetY() const
{
	return this->y;
}

/// <summary>
/// Get room width
/// </summary>
/// <returns></returns>
int Room::GetWidth() const
{
	return width;
}

/// <summary>
/// Get Room height
/// </summary>
/// <returns></returns>
int Room::GetHeight() const
{
	return height;
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

// Walls dont respont to frame updates yet
void Room::Update(float deltaMs)
{
}

/// <summary>
/// Remove a wall
/// </summary>
/// <param name="wall">wall index</param>
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

/// <summary>
/// Add a wall if not already added
/// </summary>
/// <param name="wall"></param>
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

/// <summary>
/// Remove a wall
/// </summary>
/// <param name="wall">wall index</param>
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

/// <summary>
/// Get name
/// </summary>
/// <returns></returns>
string Room::GetName()
{
	return "Room";
}
