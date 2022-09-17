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
	
Player::Player(const int x, const int y, const int playerWidth, const int playerHeight, const std::string inIdentifier) : DrawableGameObject(x, y, true), GameObjectsPtr(SceneManager::Get()->GetGameWorld().GetGameObjects())
{	
	width = playerWidth;
	height = playerHeight;
	sprite = nullptr;
	currentFacingDirection = this->currentMovingDirection;
	Identifier = inIdentifier;
	
	SubscribeToEvent(EventType::ControllerMoveEvent);
	SubscribeToEvent(EventType::SettingsReloaded);
	SubscribeToEvent(EventType::DoLogicUpdateEventType);
	SubscribeToEvent(EventType::Fire);

	ignoreRestrictions = false;
	verbose = false;

	moveDurationMs = SettingsManager::Get()->GetInt("player", "moveDurationMs");
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
			LogMessage("Fire!", verbose);
			Fire();
			break;
		case EventType::SettingsReloaded:
			LogMessage("Reloading player settings", verbose);
			LoadSettings();
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
	SetPlayerDirection(controllerMoveEvent->Direction);
	
	AddToPlayerMovements(controllerMoveEvent, createdEvents);
	return createdEvents;
}

void Player::AddToPlayerMovements(std::shared_ptr<gamelib::ControllerMoveEvent>& controllerMoveEvent, ListOfEvents& createdEvents)
{
	auto targetRoom = GetTargettedRoom(controllerMoveEvent, GetTopRoom(), GetBottomRoom(), GetLeftRoom(), GetRightRoom());
	auto targetRoomNumberAsString = targetRoom != nullptr ? std::to_string(targetRoom->GetRoomNumber()) : "";
	auto lastMoveIsSameAsCurrent = PlayerHasPendingMoves() ? moveQueue.back()->GetMovementTargetId() == targetRoomNumberAsString : false;

	// The player will process all its own movements in its update function
	if (!lastMoveIsSameAsCurrent)
	{
		if (debugMovement)
			Logger::Get()->LogThis("Canceling last movement.");

		// Cancel any pending moves 
		moveQueue.clear();
		moveQueue.push_back(std::shared_ptr<Movement>(new Movement(moveDurationMs, controllerMoveEvent->Direction, maxPixelsToMove, debugMovement)));
	}		

	// Player moved. Tell the world.
	createdEvents.push_back(make_shared<PlayerMovedEvent>(controllerMoveEvent->Direction));
}

void Player::Update(float deltaMs)
{
	ProcessMovements(deltaMs);
	Bounds = CalculateBounds(Position.GetX(), Position.GetY());
	UpdateSprite(deltaMs);
}

void Player::Draw(SDL_Renderer* renderer)
{
	DrawSprite(renderer);	
	DrawHotspot(renderer);
	DrawBounds(renderer);	
}

SDL_Rect Player::CalculateBounds(int x, int y)
{
	return { x, y, width, height };
}

void Player::UpdateSprite(float deltaMs)
{
	if(!sprite)
		return;
		
	SetSpriteAnimationFrameGroup();

	if (PlayerHasPendingMoves())
	{
		sprite->StartAnimation();
	}
	else
	{
		sprite->StopAnimation();
	}

	sprite->Update(deltaMs);
	sprite->MoveSprite(Position.GetX(), Position.GetY());
}

void Player::ProcessMovements(float deltaMs)
{
	// Process queued movements
	for (auto& currentMovement : moveQueue)
	{
		if (!currentMovement->IsComplete())
		{			
			currentMovement->Update(deltaMs);	
			
			// Actually move player (set player's position)
			moveStrategy->MovePlayer(currentMovement);

			OnAfterMove();
		}
	}

	/*if (!moveQueue.empty())
	{
		auto lastMove = moveQueue.back();
		auto targetRoom = GameData::Get()->GetRoom(std::atoi(lastMove->GetMovementTargetId().c_str()));
		if (!IsWithinRoom(CurrentRoom) && !IsWithinRoom(targetRoom))
		{
			moveStrategy->MovePlayerTo()
		}
	}*/

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
	moveDurationMs = SettingsManager::Get()->GetInt("player", "moveDurationMs");
	maxPixelsToMove = SettingsManager::Get()->GetInt("player", "maxPixelsToMove");	
	hotspotSize = SettingsManager::Get()->GetInt("player", "hotspotSize");
	drawHotSpot = SettingsManager::Get()->GetBool("player", "drawHotspot");
	hideSprite = SettingsManager::Get()->GetBool("player", "hideSprite");
}

void Player::OnAfterMove()
{
	// Check if player is within the target room yet
	/*if (IsWithinRoom(targetRoom))
	{
		SetPlayerRoom(targetRoom->GetRoomNumber());
	}*/

	// Update the list of current restrictions based on current position of player
	//SetRoomRestrictions();
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
	
	/*restrictions.CanMoveRight = moveStrategy->CanPlayerMoveRight(currentMovingDirection == Direction::Right, CurrentRoom, restrictions.RightRoom);
	restrictions.CanMoveLeft = moveStrategy->CanPlayerMoveLeft(currentMovingDirection == Direction::Left, CurrentRoom, restrictions.LeftRoom);
	restrictions.CanMoveDown = moveStrategy->CanPlayerMoveDown(currentMovingDirection == Direction::Down, CurrentRoom, restrictions.BottomRoom);
	restrictions.CanMoveUp = moveStrategy->CanPlayerMoveUp(currentMovingDirection == Direction::Up, CurrentRoom, restrictions.TopRoom);*/
}


bool Player::IsValidMove(const Direction& moveDirection, const bool& canMoveDown, const bool& canMoveLeft, const bool& canMoveRight, const bool& canMoveUp, std::shared_ptr<Movement> movement)
{	
	if(ignoreRestrictions)
	{
		Logger::Get()->LogThis("Ignoring all movement restrictions.");
		return true;
	}

	return moveDirection == Direction::Down && canMoveDown || 
		moveDirection == Direction::Left && canMoveLeft || 
		moveDirection == Direction::Right && canMoveRight ||
		moveDirection == Direction::Up && canMoveUp;
}

bool Player::IsValidMove(std::shared_ptr<Movement> movement)
{
	if (ignoreRestrictions)
	{
		Logger::Get()->LogThis("Ignoring all movement restrictions.");
		return true;
	}

	auto currentRoom = GetCurrentRoom();
	auto topRoom = GetAdjacentRoomTo(currentRoom, Side::Top);
	auto rightRoom = GetAdjacentRoomTo(currentRoom, Side::Right);
	auto bottomRoom = GetAdjacentRoomTo(currentRoom, Side::Bottom);
	auto leftRoom = GetAdjacentRoomTo(currentRoom, Side::Left);

	switch (movement->direction)
	{
	case Direction::Down:
		return moveStrategy->CanPlayerMoveDown(currentRoom, bottomRoom, movement);
		break;
	case Direction::Left:
		return moveStrategy->CanPlayerMoveLeft(currentRoom, leftRoom, movement);
		break;
	case Direction::Right:
		return moveStrategy->CanPlayerMoveRight(currentRoom, rightRoom, movement);
		break;
	case Direction::Up:
		return moveStrategy->CanPlayerMoveUp(currentRoom, topRoom, movement);
		break;
	}
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

const shared_ptr<Room> Player::GetAdjacentRoomTo(shared_ptr<Room> room, Side side)
{
	return GameData::Get()->GetRoom(room->GetNeighbourIndex(side));
}

gamelib::coordinate<int> Player::CalculateHotspotPosition(int x, int y)
{
	auto mid_x = x + GetWidth()/2;
	auto mid_y = y + GetHeight()/2;
	return coordinate<int>(mid_x, mid_y);
}

int Player::GetHotSpotLength()
{
	return hotspotSize;
}

gamelib::coordinate<int> Player::GetHotspot()
{
	return CalculateHotspotPosition(Position.GetX(), Position.GetY());
}

bool Player::IsWithinRoom(std::shared_ptr<Room> room)
{
	auto x1 = GetHotspot().GetX();
	auto y1 = GetHotspot().GetY();
	return SDL_IntersectRectAndLine(&room->InnerBounds, &x1, &y1, &x1, &y1);
}

void Player::SetPlayerRoom(int roomIndex) 
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

void Player::SetMoveStrategy(shared_ptr<IPlayerMoveStrategy> moveStrategy)
{
	this->moveStrategy = moveStrategy;
}

void Player::RemovePlayerFacingWall()
{		
	switch(currentFacingDirection)
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
	GetRightNeighbourRoom()->RemoveWall(Side::Left);
}

void Player::RemoveLeftWall()
{
	CurrentRoom->RemoveWall(Side::Left);
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
	if (!hideSprite)
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
	if (drawHotSpot)
	{		
		SDL_Rect point_bounds =
		{
			GetHotspot().GetX(), 
			GetHotspot().GetY(), 
			GetHotSpotLength(),
			GetHotSpotLength()
		};
		DrawFilledRect(renderer, &point_bounds, { 255, 0 ,0 ,0 });
	}
}

string Player::GetName()
{
	return Identifier;
}

GameObjectType Player::GetGameObjectType() 
{ 
	return GameObjectType::Player;
}

void Player::Fire()
{
	RemovePlayerFacingWall();	
}

std::shared_ptr<Room> Player::GetTargettedRoom(std::shared_ptr<gamelib::ControllerMoveEvent> positionChangedEvent, std::shared_ptr<Room> topRoom, std::shared_ptr<Room> bottomRoom, std::shared_ptr<Room> leftRoom, std::shared_ptr<Room> rightRoom)
{
	std::shared_ptr<Room> moveTowardsRoom = nullptr;

	switch (positionChangedEvent->Direction)
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
	// local func to the center the player in the given room
	const function<coordinate<int>(Room, Player)> centerPlayerFunc = [](const Room& room, Player p)
	{
		auto const room_x_mid = room.GetX() + (room.GetWidth() / 2);
		auto const room_y_mid = room.GetY() + (room.GetHeight() / 2);
		auto const x = room_x_mid - p.width / 2;
		auto const y = room_y_mid - p.height / 2;
		return coordinate<int>(x, y);
	};

	const auto coords = centerPlayerFunc(*targetRoom, *this);
	Position.SetY(coords.GetY());
	Position.SetX(coords.GetX());
}

bool Player::PlayerHasPendingMoves()
{
	return !moveQueue.empty();
}

shared_ptr<Room> Player::GetTopRoom()
{
	return GetAdjacentRoomTo(GetCurrentRoom(), Side::Top);
}
shared_ptr<Room> Player::GetBottomRoom()
{
	return GetAdjacentRoomTo(GetCurrentRoom(), Side::Bottom);
}
shared_ptr<Room> Player::GetRightRoom()
{
	return GetAdjacentRoomTo(GetCurrentRoom(), Side::Right);
}
shared_ptr<Room> Player::GetLeftRoom()
{
	return GetAdjacentRoomTo(GetCurrentRoom(), Side::Left);
}