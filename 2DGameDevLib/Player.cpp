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
#include <events/UpdateAllGameObjectsEvent.h>
#include "GameData.h"
#include <events/EventFactory.h>

using namespace std;
using namespace gamelib;
	
// Create a player 
Player::Player(gamelib::coordinate<int> position, const int width, const int height, const std::string inIdentifier) 
	: DrawableGameObject(position, true), GameObjectsPtr(SceneManager::Get()->GetGameWorld().GetGameObjects())
{	
	CommonInit(width, height, inIdentifier);
}

Player::Player(std::shared_ptr<Room> room, int width, int height, std::string identifier) : DrawableGameObject(room->GetCenter(width, height), true), 
	GameObjectsPtr(SceneManager::Get()->GetGameWorld().GetGameObjects())
{
	CommonInit(width, height, identifier);
	SetPlayerRoom(room->GetRoomNumber());
	CenterPlayerInRoom(room);
}

void Player::CommonInit(const int inWidth, const int inHeight, const std::string inIdentifier)
{
	width = inWidth;
	height = inHeight;
	sprite = nullptr;
	currentFacingDirection = this->currentMovingDirection;
	Identifier = inIdentifier;

	SubscribeToEvent(EventType::ControllerMoveEvent);
	SubscribeToEvent(EventType::SettingsReloaded);
	SubscribeToEvent(EventType::Fire);
}

ListOfEvents Player::HandleEvent(const shared_ptr<Event> event)
{
	ListOfEvents createdEvents;

	BaseProcessEvent(event, createdEvents);
	
	switch (event->type)
	{		
		case EventType::ControllerMoveEvent:
		{
			return OnControllerMove(event, createdEvents);
		}
		break;	
		case EventType::Fire:
		{
			LogMessage("Fire!", verbose);
			Fire();
		}
		break;
		case EventType::SettingsReloaded:
		{
			LogMessage("Reloading player settings", verbose);
			LoadSettings();
		}
		break;
		case EventType::InvalidMove:
		{
			LogMessage("Invalid move", verbose);
		}
		break;
	}

	return createdEvents;
}


const ListOfEvents& Player::OnControllerMove(const shared_ptr<Event>& event, ListOfEvents& createdEvents)
{
	auto moveDetails = dynamic_pointer_cast<ControllerMoveEvent>(event);	
	
	AddToPlayerMovements(moveDetails, createdEvents); // NB: movements will be processed udirng updates

	return createdEvents;
}

void Player::AddToPlayerMovements(std::shared_ptr<gamelib::ControllerMoveEvent>& controllerMoveEvent, ListOfEvents& createdEvents)
{
	SetPlayerDirection(controllerMoveEvent->Direction);

	auto doesPlayerHaveMoves = PlayerHasPendingMoves();
	auto lastDirectionIsSameAsCurrent = doesPlayerHaveMoves
		? moveQueue.back()->direction == controllerMoveEvent->Direction
		: false;

	if (!lastDirectionIsSameAsCurrent)
	{
		if (doesPlayerHaveMoves)
		{
			moveQueue.clear();
		}

		moveQueue.push_back(std::shared_ptr<Movement>(new Movement(moveDurationMs, controllerMoveEvent->Direction, maxPixelsToMove, debugMovement)));
	}
}

void Player::Update(float deltaMs)
{	
	Move(deltaMs);

	Bounds = CalculateBounds(Position.GetX(), Position.GetY()); // Calculate new bounds based on position
}

void Player::Draw(SDL_Renderer* renderer)
{
	DrawSprite(renderer);	
	DrawHotspot(renderer);
	DrawBounds(renderer);	
}

SDL_Rect Player::CalculateBounds(int x, int y) { return { x, y, width, height }; }

void Player::UpdateSprite(float deltaMs)
{
	if (!sprite)
	{
		return;
	}
		
	SetSpriteAnimationFrameGroup();

	if (PlayerHasPendingMoves())
	{
		sprite->EnableAnimation();
	}
	else
	{
		sprite->DisableAnimation();
	}

	sprite->Update(deltaMs);
}

void Player::Move(float deltaMs)
{
	for (auto& currentMovement : moveQueue)
	{
		if (!currentMovement->IsComplete())
		{
			currentMovement->Update(deltaMs);	
			
			if (!moveStrategy->MovePlayer(currentMovement))
			{
				EventManager::Get()->RaiseEvent(EventFactory::Get()->CreateGenericEvent(gamelib::EventType::InvalidMove), this);
			}

			if (currentMovement->IsComplete())
			{
				EventManager::Get()->RaiseEvent(EventFactory::Get()->CreatePlayerMovedEvent(currentMovement->direction), this);
			}
		}
	}

	// Remove moves if they have all been completed.
	moveQueue.erase(std::remove_if(moveQueue.begin(), moveQueue.end(), [&](const std::shared_ptr<Movement> movement)-> bool 
	{ 
		return movement->IsComplete(); 
	}), moveQueue.end());	

	UpdateSprite(deltaMs);

	sprite->MoveSprite(Position.GetX(), Position.GetY()); // Move the sprite to match the position of the player
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
	debugMovement = gamelib::SettingsManager::Get()->GetBool("player", "debugMovement");
	verbose = SettingsManager::Get()->GetBool("global", "verbose");
	moveDurationMs = SettingsManager::Get()->GetInt("player", "moveDurationMs");
	maxPixelsToMove = SettingsManager::Get()->GetInt("player", "maxPixelsToMove");	
	hotspotSize = SettingsManager::Get()->GetInt("player", "hotspotSize");
	drawHotSpot = SettingsManager::Get()->GetBool("player", "drawHotspot");
	hideSprite = SettingsManager::Get()->GetBool("player", "hideSprite");
}

void Player::SetPlayerDirection(Direction direction)
{
	currentMovingDirection = direction;
	currentFacingDirection = direction;
}	

inline const std::shared_ptr<Room> Player::GetCurrentRoom()
{
	return GameData::Get()->GetRoomByIndex(playerRoomIndex);
}

const std::shared_ptr<Room> Player::GetRoomByIndex(int index)
{
	return GameData::Get()->GetRoomByIndex(index);
}

const shared_ptr<Room> Player::GetAdjacentRoomTo(shared_ptr<Room> room, Side side)
{
	return GameData::Get()->GetRoomByIndex(room->GetNeighbourIndex(side));
}

gamelib::coordinate<int> Player::CalculateHotspotPosition(int x, int y)
{
	auto mid_x = x + GetWidth()/2;
	auto mid_y = y + GetHeight()/2;
	return coordinate<int>(mid_x, mid_y);
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
	GetRightRoom()->RemoveWall(Side::Left);
}

void Player::RemoveLeftWall()
{
	CurrentRoom->RemoveWall(Side::Left);
	GetLeftRoom()->RemoveWall(Side::Right);
}

void Player::RemoveBottomWall()
{
	CurrentRoom->RemoveWall(Side::Bottom);
	GetBottomRoom()->RemoveWall(Side::Top);
}

void Player::RemoveTopWall()
{
	CurrentRoom->RemoveWall(Side::Top);
	GetTopRoom()->RemoveWall(Side::Bottom);
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

void Player::Fire()
{
	RemovePlayerFacingWall();	
}

void Player::CenterPlayerInRoom(shared_ptr<Room> targetRoom)
{
	// local func
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