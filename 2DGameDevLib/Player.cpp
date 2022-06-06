#include "pch.h"
#include "Player.h"
#include "PlayerComponent.h"
#include <memory>
#include <objects/game_world_component.h>
#include "common/Common.h"
#include "common/constants.h"
#include "events/PlayerMovedEvent.h"
#include "events/ControllerMoveEvent.h"
#include "scene/SceneManager.h"
#include "Room.h"
#include <events/Event.h>
#include <functional>
#include <events/DoLogicUpdateEvent.h>

using namespace std;
using namespace gamelib;
	
Player::Player(const int x, const int y, const int w, const int h) 
	: DrawableGameObject(x, y, true), width(w), height(h), currentMovingDirection(Direction::Up), sprite(nullptr), 
	GameObjectsPtr(SceneManager::Get()->GetGameWorld().GetGameObjects()),
	currentFacingDirection(gamelib::Direction::Down)
{	
	// Player will respond to directional keyboard input
	SubscribeToEvent(EventType::ControllerMoveEvent);

	// Player will be able to have its settings re-loaded at runtime
	SubscribeToEvent(EventType::SettingsReloaded);

	// Player can update itself 
	SubscribeToEvent(EventType::DoLogicUpdateEventType);

	SubscribeToEvent(EventType::Fire);

	animationTimeoutTimer = Timer();
}

vector<shared_ptr<Event>> Player::HandleEvent(const shared_ptr<Event> event)
{
	vector<shared_ptr<Event>> createdEvents;

	BaseProcessEvent(event, createdEvents);
	
	switch (event->type)
	{
		case EventType::ControllerMoveEvent:
			return OnControllerMove(event, createdEvents);
			break;
		case EventType::DoLogicUpdateEventType:
			Update(dynamic_pointer_cast<LogicUpdateEvent>(event)->deltaMs);
			break;
		case EventType::Fire:
			Fire();
			break;
	}

	return createdEvents;
}

void Player::Fire()
{
	RemoveWall(currentFacingDirection);	
}

void Player::RemoveWall(gamelib::Direction facingDirection)
{
	switch(facingDirection)
	{
		case gamelib::Direction::Up:		
			RemoveTopWall();
			break;
		case gamelib::Direction::Down:
			RemoveBottomWall();
			break;
		case gamelib::Direction::Left:
			RemoveLeftWall();
			break;
		case gamelib::Direction::Right:
			RemoveRightWall();
			break;
	}
}

void Player::RemoveRightWall()
{
	CurrentRoom->RemoveWall(Side::Right);
	GetRoom(CurrentRoom->GetNeighbourIndex(Side::Right))->RemoveWall(Side::Left);
}

void Player::RemoveLeftWall()
{
	CurrentRoom->RemoveWall(Side::Left);
	GetRoom(CurrentRoom->GetNeighbourIndex(Side::Left))->RemoveWall(Side::Right);
}

void Player::RemoveBottomWall()
{
	CurrentRoom->RemoveWall(Side::Bottom);
	GetRoom(CurrentRoom->GetNeighbourIndex(Side::Bottom))->RemoveWall(Side::Top);
}

void Player::RemoveTopWall()
{
	CurrentRoom->RemoveWall(Side::Top);
	GetRoom(CurrentRoom->GetNeighbourIndex(Side::Top))->RemoveWall(Side::Bottom);
}

void Player::BaseProcessEvent(const shared_ptr<Event>& event, events& createdEvents)
{
	for (auto& createdEvent : GameObject::HandleEvent(event))
	{
		createdEvents.push_back(createdEvent);
	}
}

const events& Player::OnControllerMove(const shared_ptr<Event>& event, events& createdEvents)
{
	auto positionChangedEvent = dynamic_pointer_cast<ControllerMoveEvent>(event);
	auto gameObjects = SceneManager::Get()->GetGameWorld().GetGameObjects();
	
	auto currentRoom = GetCurrentRoom(gameObjects);
	auto topRoom = GetRoom(gameObjects, currentRoom, Side::Top);
	auto rightRoom = GetRoom(gameObjects, currentRoom, Side::Right);
	auto bottomRoom = GetRoom(gameObjects, currentRoom, Side::Bottom);
	auto leftRoom = GetRoom(gameObjects, currentRoom, Side::Left);

	const auto isMovingRight = positionChangedEvent->direction == Direction::Right;
	const auto isMovingDown = positionChangedEvent->direction == Direction::Down;
	const auto isMovingUp = positionChangedEvent->direction == Direction::Up;
	const auto isMovingLeft = positionChangedEvent->direction == Direction::Left;

	const auto canMoveRight = moveStrategy->CanMoveRight(isMovingRight, currentRoom, rightRoom);
	const auto canMoveLeft = moveStrategy->CanMoveLeft(isMovingLeft, currentRoom, leftRoom);
	const auto canMoveDown = moveStrategy->CanMoveDown(isMovingDown, currentRoom, bottomRoom);
	const auto canMoveUp = moveStrategy->CanMoveUp(isMovingUp, currentRoom, topRoom);

	// An invalid move is when we want to move in a direction but we cant
	auto ignoreRestrictions = SettingsManager::Get()->GetBool("player", "ignore-restrictions");
	if(!ignoreRestrictions)
	{
		if (!IsValidMove(positionChangedEvent->direction, canMoveDown, canMoveLeft, canMoveRight, canMoveUp))
		{	

			LogMessage("Invalid move", SettingsManager::Get()->GetBool("global", "verbose"));
			EventManager::Get()->RaiseEvent(make_shared<Event>(EventType::InvalidMove), this);
			
			currentFacingDirection = positionChangedEvent->direction;
			return createdEvents;
		}
	}

	MovePlayer(isMovingDown, bottomRoom, isMovingUp, topRoom, isMovingRight, rightRoom, isMovingLeft, leftRoom);

	currentFacingDirection = positionChangedEvent->direction;

	// Player moved. Tell the world.
	createdEvents.push_back(make_shared<PlayerMovedEvent>(positionChangedEvent->direction));

	return createdEvents;
}

void Player::LoadSettings()
{
	GameObject::LoadSettings();
	drawBox = SettingsManager::Get()->GetBool("player", "draw_box");
}

void Player::CenterPlayerInRoom(shared_ptr<Room> targetRoom)
{
	// Function to the center the player in the given room
	const function<coordinate<int>(Room, Player)> centerPlayerFunc = [](const Room& room, Player p)
	{
		auto const room_x_mid = room.GetX() + (room.GetWidth() / 2);
		auto const room_y_mid = room.GetY() + (room.GetHeight() / 2);
		auto const x = room_x_mid - p.width / 2;
		auto const y = room_y_mid - p.height / 2;
		return coordinate<int>(x, y);
	};

	const auto coords = centerPlayerFunc(*targetRoom, *this);
	y = coords.GetY();
	x = coords.GetX();
	Update(0.0f);
}

void Player::MovePlayer(const bool& isMovingDown, shared_ptr<Room>& bottomRoom, const bool& isMovingUp, shared_ptr<Room>& aboveRoom, const bool& isMovingRight, shared_ptr<Room>& rightRoom, const bool& isMovingLeft, shared_ptr<Room>& leftRoom)
{
	if (isMovingDown)
	{
		SetMovingDirection(Direction::Down);
		moveStrategy->MoveTo(bottomRoom);
	}

	if (isMovingUp)
	{
		SetMovingDirection(Direction::Up);
		moveStrategy->MoveTo(aboveRoom);
	}

	if (isMovingRight)
	{
		SetMovingDirection(Direction::Right);
		moveStrategy->MoveTo(rightRoom);
	}

	if (isMovingLeft)
	{
		SetMovingDirection(Direction::Left);
		moveStrategy->MoveTo(leftRoom);		
	}
		
	CurrentRoom = GetCurrentRoom();

	restrictions.IsMovingRight = isMovingRight;
	restrictions.IsMovingDown = isMovingDown;
	restrictions.IsMovingUp = isMovingUp;
	restrictions.IsMovingLeft = isMovingLeft;
	restrictions.IsMoving = restrictions.IsMovingRight || restrictions.IsMovingDown || restrictions.IsMovingUp || restrictions.IsMovingLeft;

	SetRoomRestrictions();

	if(restrictions.IsMoving)
	{
		sprite->StartAnimation();
		animationTimeoutTimer.Reset();
	}

}

void Player::SetRoomRestrictions()
{
	if(!CurrentRoom)
	{
		CurrentRoom = GetCurrentRoom();
	}

	auto gameObjects = SceneManager::Get()->GetGameWorld().GetGameObjects();
	restrictions.TopRoom = GetRoom(gameObjects, CurrentRoom, Side::Top);
	restrictions.RightRoom = GetRoom(gameObjects, CurrentRoom, Side::Right);
	restrictions.BottomRoom = GetRoom(gameObjects, CurrentRoom, Side::Bottom);
	restrictions.LeftRoom = GetRoom(gameObjects, CurrentRoom, Side::Left);

	restrictions.CanMoveRight = moveStrategy->CanMoveRight(true, CurrentRoom, restrictions.RightRoom);
	restrictions.CanMoveLeft = moveStrategy->CanMoveLeft(true, CurrentRoom, restrictions.LeftRoom);
	restrictions.CanMoveDown = moveStrategy->CanMoveDown(true, CurrentRoom, restrictions.BottomRoom);
	restrictions.CanMoveUp = moveStrategy->CanMoveUp(true, CurrentRoom, restrictions.TopRoom);
}

void Player::SetMovingDirection(Direction direction)
{
	this->currentMovingDirection = direction;
}	

const ptrdiff_t Player::CountRoomGameObjects(vector<shared_ptr<GameObject>>& gameObjects)
{
	return count_if(begin(gameObjects), end(gameObjects), [](weak_ptr<GameObject> gameObject)
	{
		if (auto ptr = gameObject.lock())
		{
			return ptr->GetGameObjectType() == GameObjectType::Room;
		}
		return false;
	});
}

const shared_ptr<Room> Player::GetCurrentRoom(vector<shared_ptr<GameObject>>& gameObjects)
{
	return dynamic_pointer_cast<Room>(gameObjects[playerRoomIndex]);
}

const std::shared_ptr<Room> Player::GetCurrentRoom()
{
	auto gameObjects = SceneManager::Get()->GetGameWorld().GetGameObjects();
	return dynamic_pointer_cast<Room>(gameObjects[playerRoomIndex]);
}

const std::shared_ptr<Room> Player::GetRoom(int index)
{
	auto gameObjects = SceneManager::Get()->GetGameWorld().GetGameObjects();
	return dynamic_pointer_cast<Room>(gameObjects[index]);
}

const shared_ptr<Room> Player::GetRoom(vector<shared_ptr<GameObject>>& gameObjects, shared_ptr<Room>& currentRoom, Side side)
{
	return dynamic_pointer_cast<Room>(gameObjects[currentRoom->GetNeighbourIndex(side)]);
}

bool Player::IsValidMove(const Direction& moveDirection, const bool& canMoveDown, const bool& canMoveLeft, const bool& canMoveRight, const bool& canMoveUp)
{
	return moveDirection == Direction::Down && canMoveDown ||
		moveDirection == Direction::Left && canMoveLeft ||
		moveDirection == Direction::Right && canMoveRight ||
		moveDirection == Direction::Up && canMoveUp;
}

gamelib::coordinate<int> Player::CalculateHotspot(int x, int y)
{
	auto mid_x = x + GetWidth()/2;
	auto mid_y = y + GetHeight()/2;
	return coordinate<int>(mid_x, mid_y);
}

gamelib::coordinate<int> Player::GetHotspot()
{
	return CalculateHotspot(x,y);
}

void Player::Draw(SDL_Renderer* renderer)
{
	if(!SettingsManager::Get()->GetBool("player", "hideSprite"))
		sprite->Draw(renderer);

	// Draw the hotspot
	if(SettingsManager::Get()->GetBool("player", "drawHotspot"))
	{
		auto hotspotSize = SettingsManager::Get()->GetInt("player", "hotspotSize");
		SDL_Rect point_bounds = 
		{ 
			GetHotspot().GetX(),// -hotspotSize, 
			GetHotspot().GetY()// +hotspotSize 
		};
		DrawFilledRect(renderer, &point_bounds , { 255, 0 ,0 ,0 });
	}

	if(drawBox)
	{
		SDL_Color colour = { 255, 0 ,0 ,0 };
		SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
		SDL_RenderDrawRect(renderer, &Bounds);
	}

	
}

string Player::GetName()
{
	return "player";
}

GameObjectType Player::GetGameObjectType() 
{ 
	return GameObjectType::Player;
}

void Player::Update(float deltaMs)
{
	Bounds = { 	x, 	y, width, height	};

	// if facing up use keyframes tagged with up
	if (sprite)
	{
		switch (currentFacingDirection)
		{
		case Direction::Up:
			sprite->SetAnimationFrameGroup("up");
			break;
		case Direction::Right:
			sprite->SetAnimationFrameGroup("right");
			break;
		case Direction::Down:
			sprite->SetAnimationFrameGroup("down");
			break;
		case Direction::Left:
			sprite->SetAnimationFrameGroup("left");
			break;
		}
		
		sprite->x = x;
		sprite->y = y;
		sprite->Update(deltaMs);
	}

	if(animationTimeoutTimer.IsElapsed || animationTimeoutTimer.IsStopped)
	{
		sprite->StopAnimation();
	}

	animationTimeoutTimer.Update(deltaMs);
	

}

void Player::SetRoom(int roomIndex) 
{ 
	playerRoomIndex = roomIndex; 	
	CurrentRoom = dynamic_pointer_cast<Room>(SceneManager::Get()->GetGameWorld().GetGameObjects()[playerRoomIndex]);
	SetRoomRestrictions();
}

int Player::GetWidth() 
{ 
	return width; 
}
	
int Player::GetHeight()
{
	return height;
}

Direction Player::GetDirection()
{
	return Direction();
}

void Player::SetSprite(shared_ptr<AnimatedSprite> sprite)
{
	this->sprite = sprite;
}

void Player::SetMoveStrategy(shared_ptr<IMoveStrategy> moveStrategy)
{
	this->moveStrategy = moveStrategy;
}



