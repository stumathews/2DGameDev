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
	_sprite = nullptr;
	currentFacingDirection = this->currentMovingDirection;
	Identifier = inIdentifier;
	Hotspot = std::shared_ptr<gamelib::Hotspot>(new gamelib::Hotspot(Position, width, height, width/2));

	SubscribeToEvent(EventType::ControllerMoveEvent);
	SubscribeToEvent(EventType::SettingsReloaded);
	SubscribeToEvent(EventType::Fire);
}

ListOfEvents Player::HandleEvent(const shared_ptr<Event> event, unsigned long deltaMs)
{
	ListOfEvents createdEvents;

	BaseProcessEvent(event, createdEvents, deltaMs);
	
	switch (event->type)
	{		
		case EventType::ControllerMoveEvent:
		{
			return OnControllerMove(event, createdEvents);
		}
		break;	
		case EventType::Fire:
		{
			LogMessage("Fire!", _verbose);
			Fire();
		}
		break;
		case EventType::SettingsReloaded:
		{
			LogMessage("Reloading player settings", _verbose);
			LoadSettings();
		}
		break;
		case EventType::InvalidMove:
		{
			LogMessage("Invalid move", _verbose);
		}
		break;
	}

	return createdEvents;
}


const ListOfEvents& Player::OnControllerMove(const shared_ptr<Event>& event, ListOfEvents& createdEvents)
{
	auto moveDetails = dynamic_pointer_cast<ControllerMoveEvent>(event);	
	
	SetPlayerDirection(moveDetails->Direction);

	auto move = std::shared_ptr<Movement>(new Movement(moveDurationMs, moveDetails->Direction, maxPixelsToMove, debugMovement));

	if (!moveStrategy->MovePlayer(move))
	{
		
		EventManager::Get()->RaiseEvent(EventFactory::Get()->CreateGenericEvent(gamelib::EventType::InvalidMove), this);
	}

	_sprite->MoveSprite(Position.GetX(), Position.GetY());

	Hotspot->Update(Position);

	UpdateBounds(width, height);

	EventManager::Get()->RaiseEvent(EventFactory::Get()->CreatePlayerMovedEvent(move->GetDirection()), this);

	return createdEvents;
}

void Player::Update(float deltaMs)
{
	UpdateSprite(deltaMs);		
}

void Player::Draw(SDL_Renderer* renderer)
{
	if (!hideSprite)
	{
		_sprite->Draw(renderer);
	}

	if (drawHotSpot)
	{
		Hotspot->Draw(renderer);
	}

	if (_drawBounds)
	{
		SDL_Color colour = { 255, 0 ,0 ,0 };
		SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
		SDL_RenderDrawRect(renderer, &Bounds);
	}
}

void Player::UpdateSprite(float deltaMs)
{
	if (!_sprite)
	{
		return;
	}
		
	SetSpriteAnimationFrameGroup();

	_sprite->Update(deltaMs);
}

void Player::Move(float deltaMs)
{
	
}

void Player::SetSpriteAnimationFrameGroup()
{
	switch (currentFacingDirection)
	{
	case Direction::Up:
		_sprite->SetAnimationFrameGroup("up");
		break;
	case Direction::Right:
		_sprite->SetAnimationFrameGroup("right");
		break;
	case Direction::Down:
		_sprite->SetAnimationFrameGroup("down");
		break;
	case Direction::Left:
		_sprite->SetAnimationFrameGroup("left");
		break;
	}
}

void Player::LoadSettings()
{
	GameObject::LoadSettings();

	_drawBounds = SettingsManager::Get()->GetBool("player", "drawBounds");
	debugMovement = gamelib::SettingsManager::Get()->GetBool("player", "debugMovement");
	_verbose = SettingsManager::Get()->GetBool("global", "verbose");
	moveDurationMs = SettingsManager::Get()->GetInt("player", "moveDurationMs");
	maxPixelsToMove = SettingsManager::Get()->GetInt("player", "maxPixelsToMove");	
	hotspotSize = SettingsManager::Get()->GetInt("player", "hotspotSize");
	drawHotSpot = SettingsManager::Get()->GetBool("player", "drawHotspot");
	hideSprite = SettingsManager::Get()->GetBool("player", "hideSprite");

	Hotspot->Width = hotspotSize;
}

void Player::SetPlayerDirection(Direction direction)
{
	currentMovingDirection = direction;
	currentFacingDirection = direction;
}	

const std::shared_ptr<Room> Player::GetCurrentRoom() { return GameData::Get()->GetRoomByIndex(playerRoomIndex); }

const std::shared_ptr<Room> Player::GetRoomByIndex(int index) { return GameData::Get()->GetRoomByIndex(index); }

const shared_ptr<Room> Player::GetAdjacentRoomTo(shared_ptr<Room> room, Side side) { return GameData::Get()->GetRoomByIndex(room->GetNeighbourIndex(side)); }

bool Player::IsWithinRoom(std::shared_ptr<Room> room)
{
	auto x1 = Hotspot->GetPosition().GetX();
	auto y1 = Hotspot->GetPosition().GetY();
	return SDL_IntersectRectAndLine(&room->InnerBounds, &x1, &y1, &x1, &y1);
}

void Player::SetPlayerRoom(int roomIndex) 
{ 
	playerRoomIndex = roomIndex; CurrentRoom = dynamic_pointer_cast<Room>(SceneManager::Get()->GetGameWorld().GetGameObjects()[playerRoomIndex]);
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

void Player::RemoveRightWall() { CurrentRoom->RemoveWall(Side::Right); GetRightRoom()->RemoveWall(Side::Left); }

void Player::RemoveLeftWall() { CurrentRoom->RemoveWall(Side::Left); GetLeftRoom()->RemoveWall(Side::Right); }

void Player::RemoveBottomWall() { CurrentRoom->RemoveWall(Side::Bottom); GetBottomRoom()->RemoveWall(Side::Top); }

void Player::RemoveTopWall() {	CurrentRoom->RemoveWall(Side::Top); GetTopRoom()->RemoveWall(Side::Bottom); }

void Player::Fire() { RemovePlayerFacingWall();	}

bool Player::PlayerHasPendingMoves() { return !moveQueue.empty(); }

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
