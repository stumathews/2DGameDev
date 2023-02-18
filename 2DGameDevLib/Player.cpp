#include "pch.h"
#include "Player.h"
#include <memory>
#include "common/Common.h"
#include "events/ControllerMoveEvent.h"
#include "scene/SceneManager.h"
#include "Room.h"
#include <events/Event.h>
#include <functional>
#include "GameData.h"
#include <events/EventFactory.h>

#include "EventNumber.h"
#include "Movement/Movement.h"
#include "util/SettingsManager.h"

using namespace std;
using namespace gamelib;

Player::Player(const std::string& name, const std::string& type, const Coordinate<int> position, const int width,  // NOLINT(cppcoreguidelines-pro-type-member-init)
               const int height, const std::string
               & identifier)
	: DrawableGameObject(name, type, position, true)
{
	CommonInit(width, height, identifier); 
}

Player::Player(const std::string& name, const std::string& type, const std::shared_ptr<Room>& playerRoom,  // NOLINT(cppcoreguidelines-pro-type-member-init)
               const int playerWidth,
               const int playerHeight, const std::string
               & identifier)
	: DrawableGameObject(name, type, playerRoom->GetCenter(playerWidth, playerHeight), true)
{
	CommonInit(playerWidth, playerHeight, identifier);
	CurrentRoom = make_shared<RoomInfo>(playerRoom);
	CenterPlayerInRoom(playerRoom);
}

Player::Player(const std::string& name, const std::string& type, const std::shared_ptr<Room>& playerRoom, const std::string& identifier)  // NOLINT(cppcoreguidelines-pro-type-member-init)
	: DrawableGameObject(name, type, playerRoom->GetCenter(0, 0), true)
{
	CommonInit(0, 0, identifier); // Height / Width set by setting the asset
	CurrentRoom = make_shared<RoomInfo>(playerRoom);
	CenterPlayerInRoom(playerRoom);
}

void Player::CommonInit(const int playerWidth, const int playerHeight, const std::string& identifier)
{
	width = playerWidth;
	height = playerHeight;
	sprite = nullptr;
	currentMovingDirection = Direction::Up;
	currentFacingDirection = this->currentMovingDirection;
	Identifier = identifier;
	UpdateBounds(width, height);
	SubscribeToEvent(ControllerMoveEventId);
	SubscribeToEvent(SettingsReloadedEventId);
	SubscribeToEvent(FireEventId);
	SubscribeToEvent(GameWonEventId);
}

ListOfEvents Player::HandleEvent(const shared_ptr<Event> event, const unsigned long deltaMs)
{
	ListOfEvents createdEvents;
	BaseProcessEvent(event, createdEvents, deltaMs);

	if(event->Id.PrimaryId == ControllerMoveEventId.PrimaryId) { return OnControllerMove(event, createdEvents, deltaMs); }
	if(event->Id.PrimaryId == FireEventId.PrimaryId) { LogMessage("Fire!", verbose); Fire(); }
	if(event->Id.PrimaryId == SettingsReloadedEventId.PrimaryId) { LogMessage("Reloading player settings", verbose); 	LoadSettings();}
	if(event->Id.PrimaryId == InvalidMoveEventId.PrimaryId) { LogMessage("Invalid move", verbose);}
	if(event->Id.PrimaryId== GameWonEventId.PrimaryId) { OnGameWon(); }

	return createdEvents;
}

void Player::OnGameWon()
{
	gameWon = true;
}

const ListOfEvents& Player::OnControllerMove(const shared_ptr<Event>& event, ListOfEvents& createdEvents, const unsigned long deltaMs)
{
	if (gameWon) { return createdEvents; }
	const auto moveEvent = dynamic_pointer_cast<ControllerMoveEvent>(event);	
	
	SetPlayerDirection(moveEvent->Direction);

	// This line actually moves the player by a 'movement'
	const auto isValidMove = moveStrategy->MoveGameObject(std::make_shared<Movement>(moveEvent->Direction, pixelsToMove));

	if (!isValidMove) { EventManager::Get()->RaiseEvent(EventFactory::Get()->CreateGenericEvent(InvalidMoveEventId), this); }

	if (sprite) 
	{
		sprite->Update(deltaMs, AnimatedSprite::GetStdDirectionAnimationFrameGroup(currentFacingDirection));
		sprite->MoveSprite(Position.GetX(), Position.GetY());
	}

	Hotspot->Update(Position);

	UpdateBounds(width, height);

	// Tell subscribers player moved
	EventManager::Get()->RaiseEvent(EventFactory::Get()->CreatePlayerMovedEvent(moveEvent->Direction), this);

	return createdEvents;
}

void Player::Update(const unsigned long deltaMs) { }

void Player::Draw(SDL_Renderer* renderer)
{
	if (!hideSprite) { sprite->Draw(renderer); }
	if (drawHotSpot) { Hotspot->Draw(renderer); }	

	if (drawBounds)
	{
		constexpr SDL_Color colour = { 255, 0 ,0 ,0 };
		SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
		SDL_RenderDrawRect(renderer, &Bounds);
	}
}

void Player::LoadSettings()
{
	GameObject::LoadSettings();

	drawBounds = SettingsManager::Get()->GetBool("player", "drawBounds");
	verbose = SettingsManager::Get()->GetBool("global", "verbose");
	pixelsToMove = SettingsManager::Get()->GetInt("player", "pixelsToMove");	
	hotspotSize = SettingsManager::Get()->GetInt("player", "hotspotSize");
	drawHotSpot = SettingsManager::Get()->GetBool("player", "drawHotspot");
	hideSprite = SettingsManager::Get()->GetBool("player", "hideSprite");
}

void Player::SetPlayerDirection(const Direction direction) { currentMovingDirection = direction; currentFacingDirection = direction; }

void Player::Fire() const { RemovePlayerFacingWall(); }


void Player::SetSprite(const std::shared_ptr<AnimatedSprite>& inSprite)
{
	sprite = inSprite;
	width = inSprite->Dimensions.GetWidth();
	height = inSprite->Dimensions.GetHeight();
	
	Hotspot = std::make_shared<gamelib::Hotspot>(Position, width, height, hotspotSize);

	CalculateBounds(Position, width, height);
}

void Player::RemovePlayerFacingWall() const
{		
	switch(currentFacingDirection)
	{
		case Direction::Up:		
			RemoveTopWall();
			break;
		case Direction::Down:
			RemoveBottomWall();
			break;
		case Direction::Left:
			RemoveLeftWall();
			break;
		case Direction::Right:
			RemoveRightWall();
			break;
		case Direction::None: break;
	}
}

void Player::RemoveRightWall() const
{
	if (const auto rightRoom = CurrentRoom->GetRightRoom()) { CurrentRoom->TheRoom->RemoveWall(Side::Right); rightRoom->RemoveWall(Side::Left); }
}

void Player::RemoveLeftWall() const
{
	if (const auto leftRoom = CurrentRoom->GetLeftRoom()) { CurrentRoom->TheRoom->RemoveWall(Side::Left); leftRoom->RemoveWall(Side::Right); }
}

void Player::RemoveBottomWall() const
{
	if (const auto bottomRoom = CurrentRoom->GetBottomRoom()) { CurrentRoom->TheRoom->RemoveWall(Side::Bottom); bottomRoom->RemoveWall(Side::Top); }
}

void Player::RemoveTopWall() const
{
	if (const auto topRoom = CurrentRoom->GetTopRoom()) { CurrentRoom->TheRoom->RemoveWall(Side::Top); topRoom->RemoveWall(Side::Bottom); }
}

void Player::BaseProcessEvent(const shared_ptr<Event>& event, ListOfEvents& createdEvents, const unsigned long deltaMs)
{
	for (auto& createdEvent : GameObject::HandleEvent(event, deltaMs)) { createdEvents.push_back(createdEvent); }
}

void Player::CenterPlayerInRoom(const shared_ptr<Room>& targetRoom)
{
	// local func
	const function<Coordinate<int>(Room, Player)> centerPlayerFunc = [](const Room& room, const Player& p)
	{
		auto const roomXMid = room.GetX() + (room.GetWidth() / 2);
		auto const roomYMid = room.GetY() + (room.GetHeight() / 2);
		auto const x = roomXMid - p.width / 2;
		auto const y = roomYMid - p.height / 2;
		return Coordinate<int>(x, y);
	};

	const auto coords = centerPlayerFunc(*targetRoom, *this);
	Position.SetY(coords.GetY());
	Position.SetX(coords.GetX());
}
