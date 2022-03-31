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

	void Player::CenterPlayerInRoom(shared_ptr<Room> target_room)
	{
		// Function to the center the player in the given room
		const std::function<coordinate<int>(Room, Player)> center_player = [](const Room& room, Player p)
		{
			auto const room_x_mid = room.GetX() + (room.GetWidth() / 2);
			auto const room_y_mid = room.GetY() + (room.GetHeight() / 2);
			auto const x = room_x_mid - p.width /2;
			auto const y = room_y_mid - p.height /2;			
			return coordinate<int>(x, y);
		};
		
		const auto coords = center_player(*target_room, *this);
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
			const auto positionChangedEvent = dynamic_pointer_cast<PositionChangeEvent>(event);
			const auto moveDirection = positionChangedEvent->direction;			
					
			auto gameWorld = SceneManager::Get()->GetGameWorld();

			// Func to find last room index
			auto findLastRoomIndexFunc = [](weak_ptr<GameObject> gameObject) 
			{
				if (auto ptr = gameObject.lock()) 
				{
					return ptr->GetGameObjectType() == object_type::Room;
				}
				return false;
			};

			auto gameObjects = gameWorld.GetGameObjects();

			auto lastRoomIndex = count_if(begin(gameObjects), end(gameObjects), findLastRoomIndexFunc);
			lastRoomIndex -= 1; // we are zero based
			const auto firstRoomIndex = 0;

			auto currentRoom = dynamic_pointer_cast<Room>(gameObjects[within_room_index < firstRoomIndex ? firstRoomIndex : within_room_index]);			
			auto aboveRoom = dynamic_pointer_cast<Room>(gameObjects[GetRoomNeighbourIndex(firstRoomIndex, lastRoomIndex, 0, currentRoom)]);
			auto rightRoom = dynamic_pointer_cast<Room>(gameObjects[GetRoomNeighbourIndex(firstRoomIndex, lastRoomIndex, 1, currentRoom)]);
			auto bottomRoom = dynamic_pointer_cast<Room>(gameObjects[GetRoomNeighbourIndex(firstRoomIndex, lastRoomIndex, 2, currentRoom)]);
			auto leftRoom = dynamic_pointer_cast<Room>(gameObjects[GetRoomNeighbourIndex(firstRoomIndex, lastRoomIndex, 3, currentRoom)]);

			const auto isMovingRight = moveDirection == Direction::Right;
			const auto isMovingDown = moveDirection == Direction::Down;
			const auto isMovingUp = moveDirection == Direction::Up;
			const auto isMovingLeft = moveDirection == Direction::Left;
			
			const auto canMoveRight = isMovingRight && !currentRoom->IsWalled(Side::Right) && !rightRoom->IsWalled(Side::Left);
			const auto canMoveLeft = isMovingLeft && !currentRoom->IsWalled(Side::Left) && !leftRoom->IsWalled(Side::Right);
			const auto canMoveDown = isMovingDown && !currentRoom->IsWalled(Side::Bottom) && !bottomRoom->IsWalled(Side::Top);
			const auto canMoveUp = isMovingUp && !currentRoom->IsWalled(Side::Top)&& !aboveRoom->IsWalled(Side::Bottom);
			
			const auto isValidMove = moveDirection == Direction::Down && canMoveDown ||
											   moveDirection == Direction::Left && canMoveLeft || 
											   moveDirection == Direction::Right && canMoveRight || 
											   moveDirection == Direction::Up && canMoveUp;
			
			// An inavalid move is when we want to move in a direction but we cant
			if(!isValidMove)
			{
				LogMessage("Invalid move", be_verbose);
				EventManager::Get()->RaiseEvent(make_shared<Event>(EventType::InvalidMove), this);				
				return created_events;
			}
			
			/* Snap to Room*/
			
			if (isMovingDown)
			{
				CenterPlayerInRoom(bottomRoom);
			}
			
			if (isMovingUp)
			{
				CenterPlayerInRoom(aboveRoom);
			}
			
			if (isMovingRight)
			{
				CenterPlayerInRoom(rightRoom);
			}
			
			if (isMovingLeft)
			{
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

	

	void Player::Draw(SDL_Renderer* renderer)
	{
		SDL_Rect r = { x ,y, width, height};
		
		SDL_Color c = {
			static_cast<Uint8>(SettingsManager::Get()->GetInt("player", "r")),
			static_cast<Uint8>(SettingsManager::Get()->GetInt("player", "g")),
			static_cast<Uint8>(SettingsManager::Get()->GetInt("player", "b")),
			static_cast<Uint8>(SettingsManager::Get()->GetInt("player", "a"))			
		};
		
		DrawFilledRect(renderer, &r, c);
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

