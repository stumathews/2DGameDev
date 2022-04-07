#include "pch.h"
#include "Player.h"
#include "PlayerComponent.h"
#include <memory>
#include <objects/game_world_component.h>
#include "common/Common.h"
#include "common/constants.h"
#include "events/PlayerMovedEvent.h"
#include "events/PositionChangeEvent.h"
#include "scene/SceneManager.h"
#include "Room.h"
#include <events/Event.h>
#include <functional>

using namespace std;
using namespace gamelib;

	
Player::Player(const int x, const int y, const int w, const int h)
	:	DrawableGameObject(x, y, true), width(w), height(h)
{
	be_verbose = SettingsManager::Get()->GetBool("player", "verbose");
		
}

void Player::LoadSettings()
{
	GameObject::LoadSettings(); 		
	drawBox = SettingsManager::Get()->GetBool("player", "draw_box");
}

void Player::CenterPlayerInRoom(shared_ptr<Room> targetRoom)
{
	// Function to the center the player in the given room
	const std::function<coordinate<int>(Room, Player)> centerPlayerFunc = [](const Room& room, Player p)
	{
		auto const room_x_mid = room.GetX() + (room.GetWidth() / 2);
		auto const room_y_mid = room.GetY() + (room.GetHeight() / 2);
		auto const x = room_x_mid - p.width /2;
		auto const y = room_y_mid - p.height /2;			
		return coordinate<int>(x, y);
	};
		
	const auto coords = centerPlayerFunc(*targetRoom, *this);
	y = coords.GetY();
	x = coords.GetX();
	Update();
}

	
int Player::GetRoomNeighbourIndex(const int first_room_index, const int last_room_index, const int side, shared_ptr<Room> room)
{
		
	switch(side)
	{
	case 0: /*top*/ 
		return room->GetNeighbourIndex((Side)side) < first_room_index ? first_room_index : room->GetNeighbourIndex((Side)side);
	case 1:  /*right*/
		return room->GetNeighbourIndex((Side)side) < first_room_index ? first_room_index : room->GetNeighbourIndex((Side)side);	
	case 2:  /*bottom*/
		return room->GetNeighbourIndex((Side)side) > last_room_index ? last_room_index : room->GetNeighbourIndex((Side)side);
	case 3: /*left*/
		return room->GetNeighbourIndex((Side)side) < 0 ? 0 : room->GetNeighbourIndex((Side)side);
		default:
		return first_room_index;
	}		
}

vector<shared_ptr<Event>> Player::HandleEvent(const std::shared_ptr<Event> event)
{
	vector<shared_ptr<Event>> created_events;

	// Call base class
	for (auto& e : GameObject::HandleEvent(event))
	{
		created_events.push_back(e);
	}
		
	// Process Player events
	if(EventType::PositionChangeEventType == event->type)
	{						
		auto positionChangedEvent = dynamic_pointer_cast<PositionChangeEvent>(event);
		const auto moveDirection = positionChangedEvent->direction;	

		auto gameObjects = SceneManager::Get()->GetGameWorld().GetGameObjects();
			
		const auto firstRoomIndex = 0; 
		auto lastRoomIndex = CountRoomGameObjects(gameObjects) -1 ;
			
		auto currentRoom = GetCurrentRoom(gameObjects, firstRoomIndex);
		auto aboveRoom = GetRoom(gameObjects, firstRoomIndex, lastRoomIndex, currentRoom, 0);
		auto rightRoom = GetRoom(gameObjects, firstRoomIndex, lastRoomIndex, currentRoom, 1);
		auto bottomRoom = GetRoom(gameObjects, firstRoomIndex, lastRoomIndex, currentRoom, 2);
		auto leftRoom = GetRoom(gameObjects, firstRoomIndex, lastRoomIndex, currentRoom, 3);

		const auto isMovingRight = moveDirection == Direction::Right;
		const auto isMovingDown = moveDirection == Direction::Down;
		const auto isMovingUp = moveDirection == Direction::Up;
		const auto isMovingLeft = moveDirection == Direction::Left;
			
		const auto canMoveRight = CanMoveRight(isMovingRight, currentRoom, rightRoom);
		const auto canMoveLeft = CanMoveLeft(isMovingLeft, currentRoom, leftRoom);
		const auto canMoveDown = CanMoveDown(isMovingDown, currentRoom, bottomRoom);
		const auto canMoveUp = CanMoveUp(isMovingUp, currentRoom, aboveRoom);
			
		// Check if the move is valid
		const auto isValidMove = IsValidMove(moveDirection, canMoveDown, canMoveLeft, canMoveRight, canMoveUp);
			
		// An invalid move is when we want to move in a direction but we cant
		if(!isValidMove)
		{
			LogMessage("Invalid move", be_verbose);
			EventManager::Get()->RaiseEvent(make_shared<Event>(EventType::InvalidMove), this);				
			return created_events;
		}
			
		/* Snap to Room*/
			
		if (isMovingDown)
		{
			SetDirection(Direction::Down);
			// Put player into room below
			CenterPlayerInRoom(bottomRoom);
		}
			
		if (isMovingUp)
		{
			SetDirection(Direction::Up);
			// Put player into the room above
			CenterPlayerInRoom(aboveRoom);
		}
			
		if (isMovingRight)
		{
			SetDirection(Direction::Right);
			// Put player in the next room on th right
			CenterPlayerInRoom(rightRoom);
		}
			
		if (isMovingLeft)
		{
			SetDirection(Direction::Left);
			// Put player in the previous room on the left
			CenterPlayerInRoom(leftRoom);
		}

		// Player moved, tell the world about my move!
		created_events.push_back(make_shared<PlayerMovedEvent>(moveDirection));
	}

	if(EventType::DoLogicUpdateEventType  == event->type)
	{
		Update();
	}

	return created_events;
}

void Player::SetDirection(Direction direction)
{
	this->direction = direction;
}
	

const ptrdiff_t& Player::CountRoomGameObjects(std::vector<std::shared_ptr<gamelib::GameObject>>& gameObjects)
{
	return count_if(begin(gameObjects), end(gameObjects), [](weak_ptr<GameObject> gameObject)
	{
		if (auto ptr = gameObject.lock())
		{
			return ptr->GetGameObjectType() == object_type::Room;
		}
		return false;
	});
}

const std::shared_ptr<Room>& Player::GetCurrentRoom(std::vector<std::shared_ptr<gamelib::GameObject>>& gameObjects, const int& firstRoomIndex)
{
	auto room = gameObjects[within_room_index];
	return dynamic_pointer_cast<Room>(room);
}

const std::shared_ptr<Room>& Player::GetRoom(std::vector<std::shared_ptr<gamelib::GameObject>>& gameObjects, const int& firstRoomIndex, const ptrdiff_t& lastRoomIndex, std::shared_ptr<Room>& currentRoom, int side)
{
	auto room = gameObjects[GetRoomNeighbourIndex(firstRoomIndex, lastRoomIndex, side, currentRoom)];
	return dynamic_pointer_cast<Room>(room);
}

bool Player::IsValidMove(const gamelib::Direction& moveDirection, const bool& canMoveDown, const bool& canMoveLeft, const bool& canMoveRight, const bool& canMoveUp)
{
	return moveDirection == Direction::Down && canMoveDown ||
		moveDirection == Direction::Left && canMoveLeft ||
		moveDirection == Direction::Right && canMoveRight ||
		moveDirection == Direction::Up && canMoveUp;
}

bool Player::CanMoveUp(const bool& isMovingUp, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& aboveRoom)
{
	return isMovingUp && !currentRoom->IsWalled(Side::Top) && !aboveRoom->IsWalled(Side::Bottom);
}

bool Player::CanMoveDown(const bool& isMovingDown, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& bottomRoom)
{
	return isMovingDown && !currentRoom->IsWalled(Side::Bottom) && !bottomRoom->IsWalled(Side::Top);
}

bool Player::CanMoveLeft(const bool& isMovingLeft, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& leftRoom)
{
	return isMovingLeft && !currentRoom->IsWalled(Side::Left) && !leftRoom->IsWalled(Side::Right);
}

bool Player::CanMoveRight(const bool& isMovingRight, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& rightRoom)
{
	return isMovingRight && !currentRoom->IsWalled(Side::Right) && !rightRoom->IsWalled(Side::Left);
}

	
/// <summary>
/// Draw our player
/// </summary>
/// <param name="renderer"></param>
void Player::Draw(SDL_Renderer* renderer)
{
	// Our player is drawn as a coloured rectangle region on the screen.

	SDL_Rect region = { x ,y, width, height};
		
	SDL_Color playerColour = 
	{
		static_cast<Uint8>(SettingsManager::Get()->GetInt("player", "r")),
		static_cast<Uint8>(SettingsManager::Get()->GetInt("player", "g")),
		static_cast<Uint8>(SettingsManager::Get()->GetInt("player", "b")),
		static_cast<Uint8>(SettingsManager::Get()->GetInt("player", "a"))			
	};
		
	DrawFilledRect(renderer, &region, playerColour);
}

string Player::GetName()
{
	return "player";
}

object_type Player::GetGameObjectType() { return object_type::Player; }

void Player::Update()
{
	bounds = 
	{
		x,
		y,
		width,
		height
	};
}

void Player::SetRoom(int roomIndex) 
{ 
	within_room_index = roomIndex; 
}

int Player::GetWidth() 
{ 
	return width; 
}
	
int Player::GetHeight()
{
	return height;
}

gamelib::Direction Player::GetDirection()
{
	return gamelib::Direction();
}

