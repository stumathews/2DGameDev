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
#include "GameData.h"

using namespace std;
using namespace gamelib;
	
Player::Player(const int x, const int y, const int playerWidth, const int playerHeight) : DrawableGameObject(x, y, true), GameObjectsPtr(SceneManager::Get()->GetGameWorld().GetGameObjects())
{	
	this->width = playerWidth;
	this->height = playerHeight;
	this->currentFacingDirection = this->currentMovingDirection;
	this->sprite = nullptr;

	SubscribeToEvent(EventType::ControllerMoveEvent);
	SubscribeToEvent(EventType::SettingsReloaded);
	SubscribeToEvent(EventType::DoLogicUpdateEventType);
	SubscribeToEvent(EventType::Fire);

	ignoreRestrictions = false;
	verbose = false;
}

ListOfEvents Player::HandleEvent(const shared_ptr<Event> event)
{
	ListOfEvents createdEvents;

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
		case EventType::InvalidMove:
			LogMessage("Invalid move", verbose);
			break;
	}

	return createdEvents;
}

const ListOfEvents& Player::OnControllerMove(const shared_ptr<Event>& event, ListOfEvents& createdEvents)
{
	auto controllerMoveEvent = dynamic_pointer_cast<ControllerMoveEvent>(event);	
	auto movementDirection = controllerMoveEvent->direction;
	auto currentRoom = GetCurrentRoom();
	auto topRoom = GetAdjacentRoomTo(currentRoom, Side::Top);
	auto rightRoom = GetAdjacentRoomTo(currentRoom, Side::Right);
	auto bottomRoom = GetAdjacentRoomTo(currentRoom, Side::Bottom);
	auto leftRoom = GetAdjacentRoomTo(currentRoom, Side::Left);			
	auto moveTowardsRoom = GetTargettedRoom(controllerMoveEvent, topRoom, bottomRoom, leftRoom, rightRoom);

	SetPlayerDirection(movementDirection);
	
	if (!IsValidMove(movementDirection, 
		moveStrategy->CanMoveDown(movementDirection == Direction::Down, currentRoom, bottomRoom), 
		moveStrategy->CanMoveLeft(movementDirection == Direction::Left, currentRoom, leftRoom), 
		moveStrategy->CanMoveRight(movementDirection == Direction::Right, currentRoom, rightRoom), 
		moveStrategy->CanMoveUp(movementDirection == Direction::Up, currentRoom, topRoom)))
	{	
		createdEvents.push_back(make_shared<Event>(EventType::InvalidMove));
		return createdEvents;
	}	
	
	// Cancel last move
	// TODO: This probbaly can be improved
	if(moveQueue.size() > 0)
	{
		moveQueue.clear();
	}
	
	// Put new move in queue
	// TODO: This probably could be improved
	auto moveDurationMs = 500; // half a second
	auto InterpolateMaxPixels = 10;

	moveQueue.push_back(std::shared_ptr<Movement>(new Movement(moveDurationMs, std::to_string(moveTowardsRoom->GetRoomNumber()), InterpolateMaxPixels, debugMovement)));
		
	// Player moved. Tell the world.
	createdEvents.push_back(make_shared<PlayerMovedEvent>(movementDirection));

	return createdEvents;
}

void Player::Draw(SDL_Renderer* renderer)
{
	DrawSprite(renderer);	
	DrawHotspot(renderer);
	DrawBounds(renderer);	
}


void Player::Update(float deltaMs)
{
	ProcessMovements(deltaMs);	

	Bounds = { x, y, width, height };

	UpdateSprite(deltaMs);
}

void Player::UpdateSprite(float deltaMs)
{
	if(!sprite) { return; }
		
	SetSpriteAnimationFrameGroup();

	if (HasPendingMoves())
	{
		sprite->StartAnimation();
	}
	else
	{
		sprite->StopAnimation();
	}

	// Move the sprite with the player
	sprite->x = x;
	sprite->y = y;

	// Advance the sprite image
	sprite->Update(deltaMs);
	
}


void Player::ProcessMovements(float deltaMs)
{
	// Process queued movements
	for (auto& currentMovement : moveQueue)
	{
		if (!currentMovement->IsComplete())
		{
			// Calculate the movement in pixels to move for the amount of time that has elapsed
			currentMovement->Update(deltaMs);

			// Each movement target towards a room, represented by a room number
			auto targetRoom = GameData::Get()->GetRoom(std::atoi(currentMovement->GetMovementTargetId().c_str()));

			// Move the player towards to target room according to the calculated movement
			moveStrategy->MoveTo(targetRoom, currentMovement);

			// Check if player is within the target room yet
			if (IsWithinRoom(targetRoom))
			{
				SetRoom(targetRoom->GetRoomNumber());
			}

			// Update the list of current restrictions based on current position of player				
			OnAfterMove(currentMovingDirection);
		}
	}

	// Remove moves if they have all been completed.
	moveQueue.erase(std::remove_if(moveQueue.begin(), moveQueue.end(), [&](const std::shared_ptr<Movement> movement)-> bool 
	{ 
		return movement->IsComplete(); 
	}), moveQueue.end());
}

void Player::SetSpriteAnimationFrameGroup()
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
}

void Player::LoadSettings()
{
	GameObject::LoadSettings();

	drawBounds = SettingsManager::Get()->GetBool("player", "drawBounds");
	ignoreRestrictions = SettingsManager::Get()->GetBool("player", "ignoreRestrictions");
	debugMovement = gamelib::SettingsManager::Get()->GetBool("player", "debugMovement");
	verbose = SettingsManager::Get()->GetBool("global", "verbose");
}

void Player::OnAfterMove(const gamelib::Direction& movementDirection)
{
	SetRoomRestrictions();
}

void Player::SetRoomRestrictions()
{
	if(!CurrentRoom)
	{
		CurrentRoom = GetCurrentRoom();		
	}

	restrictions.TopRoom = GetAdjacentRoomTo(CurrentRoom, Side::Top);
	restrictions.RightRoom = GetAdjacentRoomTo(CurrentRoom, Side::Right);
	restrictions.BottomRoom = GetAdjacentRoomTo(CurrentRoom, Side::Bottom);
	restrictions.LeftRoom = GetAdjacentRoomTo(CurrentRoom, Side::Left);

	// Move tests
	
	restrictions.CanMoveRight = moveStrategy->CanMoveRight(currentMovingDirection == Direction::Right, CurrentRoom, restrictions.RightRoom);
	restrictions.CanMoveLeft = moveStrategy->CanMoveLeft(currentMovingDirection == Direction::Left, CurrentRoom, restrictions.LeftRoom);
	restrictions.CanMoveDown = moveStrategy->CanMoveDown(currentMovingDirection == Direction::Down, CurrentRoom, restrictions.BottomRoom);
	restrictions.CanMoveUp = moveStrategy->CanMoveUp(currentMovingDirection == Direction::Up, CurrentRoom, restrictions.TopRoom);
}


bool Player::IsValidMove(const Direction& moveDirection, const bool& canMoveDown, const bool& canMoveLeft, const bool& canMoveRight, const bool& canMoveUp)
{	
	if(ignoreRestrictions)
	{
		return true;
	}

	return moveDirection == Direction::Down && canMoveDown ||
		moveDirection == Direction::Left && canMoveLeft ||
		moveDirection == Direction::Right && canMoveRight ||
		moveDirection == Direction::Up && canMoveUp;
}

void Player::SetPlayerDirection(Direction direction)
{
	currentMovingDirection = direction;
	currentFacingDirection = direction;
}	

const ptrdiff_t Player::CountRoomGameObjects(ListOfGameObjects& gameObjects)
{
	return count_if(begin(gameObjects), end(gameObjects), [](std::weak_ptr<gamelib::GameObject> gameObject)
	{
		if (auto ptr = gameObject.lock())
		{
			return ptr->GetGameObjectType() == GameObjectType::Room;
		}
		return false;
	});
}

inline const std::shared_ptr<Room> Player::GetCurrentRoom()
{
	return GameData::Get()->GetRoom(playerRoomIndex);
}

const std::shared_ptr<Room> Player::GetRoom(int index)
{
	return GameData::Get()->GetRoom(index);
}

const shared_ptr<Room> Player::GetAdjacentRoomTo(shared_ptr<Room>& currentRoom, Side side)
{
	return GameData::Get()->GetRoom(currentRoom->GetNeighbourIndex(side));
}

gamelib::coordinate<int> Player::CalculateHotspotPosition(int x, int y)
{
	auto mid_x = x + GetWidth()/2;
	auto mid_y = y + GetHeight()/2;
	return coordinate<int>(mid_x, mid_y);
}

gamelib::coordinate<int> Player::GetHotspot()
{
	return CalculateHotspotPosition(x,y);
}

bool Player::IsWithinRoom(std::shared_ptr<Room> room)
{
	// Set if the player is in this room or not		
	auto x1 = GetHotspot().GetX();
	auto y1 = GetHotspot().GetY();
	return SDL_IntersectRectAndLine(&room->InnerBounds, &x1, &y1, &x1, &y1);
}

void Player::SetRoom(int roomIndex) 
{ 
	playerRoomIndex = roomIndex; 	
	CurrentRoom = dynamic_pointer_cast<Room>(SceneManager::Get()->GetGameWorld().GetGameObjects()[playerRoomIndex]);
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

void Player::RemoveWall(gamelib::Direction facingDirection)
{
	// Remove the wall that is opposite the direction the player is facing
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

	// Remove the wall on the adjacent room
	GetRightNeighbourRoom()->RemoveWall(Side::Left);
}

void Player::RemoveLeftWall()
{
	CurrentRoom->RemoveWall(Side::Left);

	// Remove the wall on the adjacent room
	GetLeftNeighbourRoom()->RemoveWall(Side::Right);
}

void Player::RemoveBottomWall()
{
	CurrentRoom->RemoveWall(Side::Bottom);
	GetBottomNeighbourRoom()->RemoveWall(Side::Top);
}

void Player::RemoveTopWall()
{
	CurrentRoom->RemoveWall(Side::Top);

	// Remove the wall on the adjacent room
	GetTopNeighbourRoom()->RemoveWall(Side::Bottom);
}

const std::shared_ptr<Room> Player::GetRightNeighbourRoom()
{
	return GetRoom(CurrentRoom->GetNeighbourIndex(Side::Right));
}

const std::shared_ptr<Room> Player::GetLeftNeighbourRoom()
{
	return GetRoom(CurrentRoom->GetNeighbourIndex(Side::Left));
}

const std::shared_ptr<Room> Player::GetBottomNeighbourRoom()
{
	return GetRoom(CurrentRoom->GetNeighbourIndex(Side::Bottom));
}

const std::shared_ptr<Room> Player::GetTopNeighbourRoom()
{
	return GetRoom(CurrentRoom->GetNeighbourIndex(Side::Top));
}

void Player::BaseProcessEvent(const shared_ptr<Event>& event, ListOfEvents& createdEvents)
{
	for (auto& createdEvent : GameObject::HandleEvent(event))
	{
		createdEvents.push_back(createdEvent);
	}
}

void Player::DrawSprite(SDL_Renderer* renderer)
{
	if (!SettingsManager::Get()->GetBool("player", "hideSprite"))
	{
		sprite->Draw(renderer);
	}
}

void Player::DrawBounds(SDL_Renderer* renderer)
{
	if (drawBounds)
	{
		SDL_Color colour = { 255, 0 ,0 ,0 };
		SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
		SDL_RenderDrawRect(renderer, &Bounds);
	}
}

void Player::DrawHotspot(SDL_Renderer* renderer)
{
	if (SettingsManager::Get()->GetBool("player", "drawHotspot"))
	{
		auto hotspotSize = SettingsManager::Get()->GetInt("player", "hotspotSize");
		SDL_Rect point_bounds =
		{
			GetHotspot().GetX(),// -hotspotSize, 
			GetHotspot().GetY()// +hotspotSize 
		};
		DrawFilledRect(renderer, &point_bounds, { 255, 0 ,0 ,0 });
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

void Player::Fire()
{
	RemoveWall(currentFacingDirection);	
}

std::shared_ptr<Room> Player::GetTargettedRoom(std::shared_ptr<gamelib::ControllerMoveEvent>& positionChangedEvent, std::shared_ptr<Room>& topRoom, std::shared_ptr<Room>& bottomRoom, std::shared_ptr<Room>& leftRoom, std::shared_ptr<Room>& rightRoom)
{
	std::shared_ptr<Room> moveTowardsRoom = nullptr;

	switch (positionChangedEvent->direction)
	{
	case Direction::Up:
		moveTowardsRoom = topRoom;
		break;
	case Direction::Down:
		moveTowardsRoom = bottomRoom;
		break;
	case Direction::Left:
		moveTowardsRoom = leftRoom;
		break;
	case Direction::Right:
		moveTowardsRoom = rightRoom;
		break;
	}

	return moveTowardsRoom;
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

bool Player::HasPendingMoves()
{
	return !moveQueue.empty();
}