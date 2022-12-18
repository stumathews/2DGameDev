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
	
Player::Player(std::string name, std::string type, gamelib::coordinate<int> position, const int width, const int height, const std::string inIdentifier)
	: DrawableGameObject(name, type, position, true)
{
	commonInit(width, height, inIdentifier); 
}

Player::Player(std::string name, std::string type, std::shared_ptr<Room> room, int width, int height, std::string identifier)
	: DrawableGameObject(name, type, room->GetCenter(width, height), true)
{
	commonInit(width, height, identifier);
	SetPlayerRoom(room);
	CenterPlayerInRoom(room);
}

void Player::commonInit(const int inWidth, const int inHeight, const std::string inIdentifier)
{
	width = inWidth;
	height = inHeight;
	sprite = nullptr;
	currentFacingDirection = this->currentMovingDirection;
	Identifier = inIdentifier;
	Hotspot = std::shared_ptr<gamelib::Hotspot>(new gamelib::Hotspot(Position, width, height, width/2));

	SubscribeToEvent(EventType::ControllerMoveEvent);
	SubscribeToEvent(EventType::SettingsReloaded);
	SubscribeToEvent(EventType::Fire);
	SubscribeToEvent(EventType::GameWon);
}

ListOfEvents Player::HandleEvent(const shared_ptr<Event> event, unsigned long deltaMs)
{
	ListOfEvents createdEvents;
	BaseProcessEvent(event, createdEvents, deltaMs);
	
	switch (event->type)
	{		
		case EventType::ControllerMoveEvent: { return OnControllerMove(event, createdEvents, deltaMs); } break;
		case EventType::Fire: { LogMessage("Fire!", _verbose); Fire(); } break;
		case EventType::SettingsReloaded: { LogMessage("Reloading player settings", _verbose); 	LoadSettings(); } break;
		case EventType::InvalidMove: { LogMessage("Invalid move", _verbose); } 	break;
		case EventType::GameWon: { OnGameWon(); }; break;
	}

	return createdEvents;
}

void Player::OnGameWon()
{
	gameWon = true;
}

const ListOfEvents& Player::OnControllerMove(const shared_ptr<Event>& event, ListOfEvents& createdEvents, unsigned long deltaMs)
{
	if (gameWon) { return createdEvents; }
	auto moveEvent = dynamic_pointer_cast<ControllerMoveEvent>(event);	
	
	SetPlayerDirection(moveEvent->Direction);

	auto isValidMove = moveStrategy->MovePlayer(std::shared_ptr<Movement>(new Movement(moveEvent->Direction, pixelsToMove)));

	if (!isValidMove) { EventManager::Get()->RaiseEvent(EventFactory::Get()->CreateGenericEvent(gamelib::EventType::InvalidMove), this); }

	if (sprite) 
	{
		sprite->Update(deltaMs, GetSpriteAnimationFrameGroupForPlayer());
		sprite->MoveSprite(Position.GetX(), Position.GetY());
	}

	Hotspot->Update(Position);

	UpdateBounds(width, height);

	// Tell subscribers player moved
	EventManager::Get()->RaiseEvent(EventFactory::Get()->CreatePlayerMovedEvent(moveEvent->Direction), this);

	return createdEvents;
}

void Player::Update(float deltaMs) { }

void Player::Draw(SDL_Renderer* renderer)
{
	if (!hideSprite) { sprite->Draw(renderer); }
	if (drawHotSpot) { Hotspot->Draw(renderer); }	

	if (_drawBounds)
	{
		SDL_Color colour = { 255, 0 ,0 ,0 };
		SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
		SDL_RenderDrawRect(renderer, &Bounds);
	}
}

std::string Player::GetSpriteAnimationFrameGroupForPlayer()
{
	switch (currentFacingDirection)
	{
		case Direction::Up: return "up"; break;
		case Direction::Right: return "right"; break;
		case Direction::Down: return "down"; break;
		case Direction::Left: return "left"; break;
	}
}

void Player::LoadSettings()
{
	GameObject::LoadSettings();

	_drawBounds = SettingsManager::Get()->GetBool("player", "drawBounds");
	_verbose = SettingsManager::Get()->GetBool("global", "verbose");
	pixelsToMove = SettingsManager::Get()->GetInt("player", "pixelsToMove");	
	hotspotSize = Hotspot->Width = SettingsManager::Get()->GetInt("player", "hotspotSize");
	drawHotSpot = SettingsManager::Get()->GetBool("player", "drawHotspot");
	hideSprite = SettingsManager::Get()->GetBool("player", "hideSprite");
}

void Player::SetPlayerDirection(Direction direction) { currentMovingDirection = direction; currentFacingDirection = direction; }	
const std::shared_ptr<Room> Player::GetCurrentRoom() { return GameData::Get()->GetRoomByIndex(playerRoomIndex); }
const std::shared_ptr<Room> Player::GetRoomByIndex(int index) { return GameData::Get()->GetRoomByIndex(index); }
const shared_ptr<Room> Player::GetAdjacentRoomTo(shared_ptr<Room> room, Side side) { return GameData::Get()->GetRoomByIndex(room->GetNeighbourIndex(side)); }
void Player::Fire() { RemovePlayerFacingWall(); }
bool Player::PlayerHasPendingMoves() { return !moveQueue.empty(); }

bool Player::IsWithinRoom(std::shared_ptr<Room> room)
{
	auto x1 = Hotspot->GetPosition().GetX();
	auto y1 = Hotspot->GetPosition().GetY();
	return SDL_IntersectRectAndLine(&room->InnerBounds, &x1, &y1, &x1, &y1);
}

void Player::SetPlayerRoom(std::shared_ptr<Room> room) 
{ 
	playerRoomIndex = room->GetRoomNumber();
	CurrentRoom = room;
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
	auto rightRoom = GetRightRoom(); 
	if (rightRoom) { CurrentRoom->RemoveWall(Side::Right); rightRoom->RemoveWall(Side::Left); }
}

void Player::RemoveLeftWall() 
{
	auto leftRoom = GetLeftRoom();
	if (leftRoom) { CurrentRoom->RemoveWall(Side::Left); leftRoom->RemoveWall(Side::Right); }
}

void Player::RemoveBottomWall() 
{ 
	auto bottomRoom = GetBottomRoom();
	if (bottomRoom) { CurrentRoom->RemoveWall(Side::Bottom); bottomRoom->RemoveWall(Side::Top); }
}

void Player::RemoveTopWall() 
{
	auto topRoom = GetTopRoom();
	if (topRoom) { CurrentRoom->RemoveWall(Side::Top); topRoom->RemoveWall(Side::Bottom); }
}

void Player::BaseProcessEvent(const shared_ptr<Event>& event, ListOfEvents& createdEvents, unsigned long deltaMs)
{
	for (auto& createdEvent : GameObject::HandleEvent(event, deltaMs)) { createdEvents.push_back(createdEvent); }
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
