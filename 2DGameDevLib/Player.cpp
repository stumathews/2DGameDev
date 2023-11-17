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
#include "character/StatefulMove.h"
#include "file/SettingsManager.h"

using namespace std;
using namespace gamelib;

Player::Player(const std::string& name,
               const std::string& type,
               const Coordinate<int> position, // we know the position the player is at
               const int width,
               const int height,
               const std::string& identifier)
	: DrawableGameObject(name, type, position, true), currentMovingDirection(Direction::Down),
	  currentFacingDirection(Direction::Down)
{
	CommonInit(width, height, identifier);
}

Player::Player(const std::string& name, const std::string& type,
               const std::shared_ptr<Room>& playerRoom, // we know the room the player will be in
               const int playerWidth,
               const int playerHeight,
               const std::string& identifier)
	: DrawableGameObject(name, type, playerRoom->GetCenter(playerWidth, playerHeight), true),
	  currentMovingDirection(Direction::Down), currentFacingDirection(Direction::Down)
{
	CommonInit(playerWidth, playerHeight, identifier);
	CurrentRoom = make_shared<RoomInfo>(playerRoom);
	CenterPlayerInRoom(playerRoom);
}

Player::Player(const std::string& name,
               const std::string& type,
               const std::shared_ptr<Room>& playerRoom,
               const std::string& identifier)
	: DrawableGameObject(name, type, playerRoom->GetCenter(0, 0), true), currentMovingDirection(Direction::Down),
	  currentFacingDirection(Direction::Down)
{
	CommonInit(0, 0, identifier); // Height / Width set by setting the asset
	CurrentRoom = make_shared<RoomInfo>(playerRoom);
	CenterPlayerInRoom(playerRoom);
}

int Player::GetHealth()
{
	return IntProperties["Health"];
}

int Player::GetPoints()
{
	return IntProperties["Points"];
}


void Player::CommonInit(const int playerWidth, const int playerHeight, const std::string& identifier)
{
	width = playerWidth;
	height = playerHeight;
	sprite = nullptr;
	currentMovingDirection = Direction::Up;
	currentFacingDirection = this->currentMovingDirection;
	Identifier = identifier;

	movementAcceleration = {
		{ Direction::Up , ControllerMoveEvent::KeyState::Unknown },
		{ Direction::Down , ControllerMoveEvent::KeyState::Unknown },
		{ Direction::Left , ControllerMoveEvent::KeyState::Unknown },
		{ Direction::Right , ControllerMoveEvent::KeyState::Unknown },
	};

	UpdateBounds(width, height);

	SubscribeToEvent(ControllerMoveEventId); // player wants to know when the controller moves
	SubscribeToEvent(SettingsReloadedEventId); // wants to know when settings are reloaded
	SubscribeToEvent(FireEventId); // wants to know when the fire button is pressed
	SubscribeToEvent(GameWonEventId); // want to know when the game is won
}

ListOfEvents Player::HandleEvent(const shared_ptr<Event> event, const unsigned long deltaMs)
{
	ListOfEvents createdEvents;
	BaseProcessEvent(event, createdEvents, deltaMs);

	if (event->Id.PrimaryId == ControllerMoveEventId.PrimaryId)
	{
		return OnControllerMove(event, createdEvents, deltaMs);
	}

	if (event->Id.PrimaryId == FireEventId.PrimaryId)
	{
		LogMessage("Fire!", verbose);
		Fire();
	}

	if (event->Id.PrimaryId == SettingsReloadedEventId.PrimaryId)
	{
		LogMessage("Reloading player settings", verbose);
		LoadSettings();
	}
	if (event->Id.PrimaryId == InvalidMoveEventId.PrimaryId) { LogMessage("Invalid move", verbose); }
	if (event->Id.PrimaryId == GameWonEventId.PrimaryId) { OnGameWon(); }

	return createdEvents;
}

void Player::OnGameWon()
{
	gameWon = true;
}

const ListOfEvents& Player::OnControllerMove(const shared_ptr<Event>& event, ListOfEvents& createdEvents, const unsigned long deltaMs)
{
	if (gameWon) return createdEvents;

	const auto moveEvent = dynamic_pointer_cast<ControllerMoveEvent>(event);
	const auto moveDirection = moveEvent->Direction;

	// Set acceleration in direction depending on if direction key is pressed or not
	movementAcceleration[moveDirection] = moveEvent->GetKeyState();
	
	SetPlayerDirection(moveDirection);

	return createdEvents;
}

void Player::Move(const unsigned long deltaMs)
{
	// This line actually moves the player by a 'movement':
	const auto movement = std::make_shared<StatefulMove>(speed, movementAcceleration, deltaMs);
	const auto isValidMove = moveStrategy->MoveGameObject(movement);

	if (!isValidMove)
	{
		EventManager::Get()->RaiseEvent(EventFactory::Get()->CreateGenericEvent(InvalidMoveEventId), this);
	}

	if (sprite)
	{

		// Animate only if moving in a certain direction (we don't have an animation for direction.None!)
		if(movement->GetDirection() != Direction::None)
		{
			sprite->Update(deltaMs, AnimatedSprite::GetStdDirectionAnimationFrameGroup(movement->GetDirection()));
		}

		sprite->MoveSprite(Position.GetX(), Position.GetY());
	}

	Hotspot->Update(Position);

	UpdateBounds(width, height);

	// Only register a move if there was a move in a known direction
	if(movement->GetDirection() != Direction::None)
	{
		EventManager::Get()->RaiseEvent(EventFactory::Get()->CreatePlayerMovedEvent(movement->GetDirection()), this);
	}
}

void Player::Update(const unsigned long deltaMs)
{
	if (GameData::Get()->IsGameWon()) return;

	moveTimer.Update(deltaMs);

	// We don't move every single frame...
	moveTimer.DoIfReady([&](){ Move(deltaMs); });
	
}

void Player::Draw(SDL_Renderer* renderer)
{
	// Draw
	if (!hideSprite) { sprite->Draw(renderer); }
	if (drawHotSpot) { Hotspot->Draw(renderer); }

	// Debugging
	if (drawBounds)
	{
		constexpr SDL_Color colour = {255, 0, 0, 0};
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
	speed = SettingsManager::Get()->Int("player", "speed");
	moveRateMs = SettingsManager::Int("enemy", "moveRateMs");
		
	moveTimer.SetFrequency(moveRateMs);
}

void Player::SetPlayerDirection(const Direction direction)
{
	currentMovingDirection = direction;
	currentFacingDirection = direction;
}

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
	switch (currentFacingDirection)
	{
		case Direction::Up: RemoveTopWall(); break;
		case Direction::Down: RemoveBottomWall(); break;
		case Direction::Left: RemoveLeftWall(); break;
		case Direction::Right: RemoveRightWall(); break;
		case Direction::None: break;
	}
}

void Player::RemoveRightWall() const
{
	if (const auto rightRoom = CurrentRoom->GetRightRoom())
	{
		CurrentRoom->TheRoom->RemoveWall(Side::Right);
		rightRoom->RemoveWall(Side::Left);
	}
}

void Player::RemoveLeftWall() const
{
	if (const auto leftRoom = CurrentRoom->GetLeftRoom())
	{
		CurrentRoom->TheRoom->RemoveWall(Side::Left);
		leftRoom->RemoveWall(Side::Right);
	}
}

void Player::RemoveBottomWall() const
{
	if (const auto bottomRoom = CurrentRoom->GetBottomRoom())
	{
		CurrentRoom->TheRoom->RemoveWall(Side::Bottom);
		bottomRoom->RemoveWall(Side::Top);
	}
}

void Player::RemoveTopWall() const
{
	if (const auto topRoom = CurrentRoom->GetTopRoom())
	{
		CurrentRoom->TheRoom->RemoveWall(Side::Top);
		topRoom->RemoveWall(Side::Bottom);
	}
}

void Player::BaseProcessEvent(const shared_ptr<Event>& event, ListOfEvents& createdEvents, const unsigned long deltaMs)
{
	for (auto& createdEvent : GameObject::HandleEvent(event, deltaMs))
	{
		createdEvents.push_back(createdEvent);
	}
}

void Player::CenterPlayerInRoom(const shared_ptr<Room>& targetRoom)
{
	// local func
	const function<Coordinate<int>(Room, Player)> centerPlayerFunc = [](const Room& room, const Player& p)
	{
		const auto roomXMid = room.GetX() + (room.GetWidth() / 2);
		const auto roomYMid = room.GetY() + (room.GetHeight() / 2);
		const auto x = roomXMid - p.width / 2;
		const auto y = roomYMid - p.height / 2;
		return Coordinate(x, y);
	};

	const auto coords = centerPlayerFunc(*targetRoom, *this);
	Position.SetY(coords.GetY());
	Position.SetX(coords.GetX());
}
