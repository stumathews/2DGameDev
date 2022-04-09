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
		
	auto& rect = GetABCDRectangle();
	const auto ax = rect.GetAx();
	const auto ay = rect.GetAy();
	const auto bx = rect.GetBx();
	const auto by = rect.GetBy();
	const auto cx = rect.GetCx();
	const auto cy = rect.GetCy();
	const auto dx = rect.GetDx();
	const auto dy = rect.GetDy();

	const auto hasTopWall = this->walls[0];
	const auto hasRightWall = this->walls[1];
	const auto hasBottomWall = this->walls[2];
	const auto hasLeftWall = this->walls[3];

	// Draw the walls as lines
	if (hasTopWall)
	{
		SDL_RenderDrawLine(renderer, ax, ay, bx, by);
	}
		
	if (hasRightWall)
	{
		SDL_RenderDrawLine(renderer, bx, by, cx, cy);
	}
		
	if (hasBottomWall)
	{
		SDL_RenderDrawLine(renderer, cx, cy, dx, dy);
	}
		
	if (hasLeftWall)
	{
		SDL_RenderDrawLine(renderer, dx, dy, ax, ay);
	}
		
	if (fill)
	{
		DrawFilledRect(renderer, &bounds, { 255, 0 ,0 ,0 });
	}
		
	if(SettingsManager::Get()->GetBool("global", "print_debugging_text"))
	{
		RectDebugging::printInRect(renderer, GetTag(), &bounds); 
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

Room::Room(int number, 
	int x, 
	int y, 
	int width, 
	int height, 
	bool fill)
	: DrawableGameObject(x, y, true), fill(fill), playerBounds({}),
		topRoomIndex(0), rightRoomIndex(0),  bottomRoomIndex(0), width(width), height(height), leftRoomIndex(0)
{
	// Bounds of this room
	this->bounds = { x, y, width, height };
	this->width = width;
	this->height = height;
	this->number = number;

	// Room geometry helper
	this->abcd = ABCDRectangle(x, y, width, height);

	// All walls are present by default
	walls[0] = true;
	walls[1] = true;
	walls[2] = true;
	walls[3] = true;
}

/// <summary>
/// Set Indexs of sorrounding rooms
/// </summary>
/// <param name="top_index"></param>
/// <param name="right_index"></param>
/// <param name="bottom_index"></param>
/// <param name="left_index"></param>
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

// Walls dont respont to frame updates yet
void Room::Update()
{
}

/// <summary>
/// Remove a wall
/// </summary>
/// <param name="wall">wall index</param>
void Room::RemoveWall(Side wall)
{
	this->walls[(int)wall] = false;
}

/// <summary>
/// Add a wall if not already added
/// </summary>
/// <param name="wall"></param>
void Room::AddWall(Side wall)
{
	this->walls[(int)wall] = true;
}

/// <summary>
/// Remove a wall
/// </summary>
/// <param name="wall">wall index</param>
void Room::RemoveWallZeroBased(Side wall)
{
	this->walls[(int)wall] = false;
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
