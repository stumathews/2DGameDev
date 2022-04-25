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

using namespace std;
using namespace gamelib;
	
Player::Player(const int x, const int y, const int w, const int h) 
	: DrawableGameObject(x, y, true), width(w), height(h), direction(Direction::Up), sprite(nullptr)
{	
	
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
			Update();
			break;
	}

	return createdEvents;
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

	const auto moveDirection = positionChangedEvent->direction;
	const auto isMovingRight = moveDirection == Direction::Right;
	const auto isMovingDown = moveDirection == Direction::Down;
	const auto isMovingUp = moveDirection == Direction::Up;
	const auto isMovingLeft = moveDirection == Direction::Left;

	const auto canMoveRight = CanMoveRight(isMovingRight, currentRoom, rightRoom);
	const auto canMoveLeft = CanMoveLeft(isMovingLeft, currentRoom, leftRoom);
	const auto canMoveDown = CanMoveDown(isMovingDown, currentRoom, bottomRoom);
	const auto canMoveUp = CanMoveUp(isMovingUp, currentRoom, topRoom);

	// An invalid move is when we want to move in a direction but we cant
	auto ignoreRestrictions = SettingsManager::Get()->GetBool("player", "ignore-restrictions");
	if(!ignoreRestrictions)
	{
		if (!IsValidMove(moveDirection, canMoveDown, canMoveLeft, canMoveRight, canMoveUp))
		{
		

			LogMessage("Invalid move", SettingsManager::Get()->GetBool("global", "verbose"));
			EventManager::Get()->RaiseEvent(make_shared<Event>(EventType::InvalidMove), this);
			return createdEvents;
	}
	}

	MovePlayer(isMovingDown, bottomRoom, isMovingUp, topRoom, isMovingRight, rightRoom, isMovingLeft, leftRoom);

	// Player moved. Tell the world.
	createdEvents.push_back(make_shared<PlayerMovedEvent>(moveDirection));

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
	Update();
}

void Player::MovePlayer(const bool& isMovingDown, shared_ptr<Room>& bottomRoom, const bool& isMovingUp, shared_ptr<Room>& aboveRoom, const bool& isMovingRight, shared_ptr<Room>& rightRoom, const bool& isMovingLeft, shared_ptr<Room>& leftRoom)
{
	if (isMovingDown)
	{
		SetDirection(Direction::Down);
		moveStrategy->MoveTo(bottomRoom);
	}

	if (isMovingUp)
	{
		SetDirection(Direction::Up);
		moveStrategy->MoveTo(aboveRoom);
	}

	if (isMovingRight)
	{
		SetDirection(Direction::Right);
		moveStrategy->MoveTo(rightRoom);
	}

	if (isMovingLeft)
	{
		SetDirection(Direction::Left);
		moveStrategy->MoveTo(leftRoom);
	}
}

void Player::SetDirection(Direction direction)
{
	this->direction = direction;
}	

const ptrdiff_t& Player::CountRoomGameObjects(vector<shared_ptr<GameObject>>& gameObjects)
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

bool Player::CanMoveUp(const bool& isMovingUp, shared_ptr<Room>& currentRoom, shared_ptr<Room>& aboveRoom)
{
	return isMovingUp && !currentRoom->IsWalled(Side::Top) && !aboveRoom->IsWalled(Side::Bottom);
}

bool Player::CanMoveDown(const bool& isMovingDown, shared_ptr<Room>& currentRoom, shared_ptr<Room>& bottomRoom)
{
	return isMovingDown && !currentRoom->IsWalled(Side::Bottom) && !bottomRoom->IsWalled(Side::Top);
}

bool Player::CanMoveLeft(const bool& isMovingLeft, shared_ptr<Room>& currentRoom, shared_ptr<Room>& leftRoom)
{
	return isMovingLeft && !currentRoom->IsWalled(Side::Left) && !leftRoom->IsWalled(Side::Right);
}

bool Player::CanMoveRight(const bool& isMovingRight, shared_ptr<Room>& currentRoom, shared_ptr<Room>& rightRoom)
{
	return isMovingRight && !currentRoom->IsWalled(Side::Right) && !rightRoom->IsWalled(Side::Left);
}

gamelib::coordinate<int> Player::GetHotspot()
{
	auto mid_x = x + GetWidth()/2;
	auto mid_y = y + GetHeight()/2;
	return coordinate<int>(mid_x, mid_y);
}

void Player::Draw(SDL_Renderer* renderer)
{		
	sprite->Draw(renderer);

	// Draw the hotspot
	SDL_Rect point_bounds = { GetHotspot().GetX() -5, GetHotspot().GetY() +5 };
	DrawFilledRect(renderer, &point_bounds , { 255, 0 ,0 ,0 });
}

string Player::GetName()
{
	return "player";
}

GameObjectType Player::GetGameObjectType() 
{ 
	return GameObjectType::Player;
}

void Player::Update()
{
	bounds = { 	x, 	y, width, height	};

	// if facing up use keyframes tagged with up
	if (sprite)
	{
		switch (direction)
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
		sprite->Update();
	}

}

void Player::SetRoom(int roomIndex) 
{ 
	playerRoomIndex = roomIndex; 
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

