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
Player::Player(const int x, const int y, const int playerWidth, const int playerHeight, const std::string inIdentifier) 
	: DrawableGameObject(x, y, true), GameObjectsPtr(SceneManager::Get()->GetGameWorld().GetGameObjects())
{	
	CommonInit(playerWidth, playerHeight, inIdentifier);
}

void Player::CommonInit(const int playerWidth, const int playerHeight, const std::string inIdentifier)
{
	// Initialize 
	width = playerWidth;
	height = playerHeight;
	sprite = nullptr;
	currentFacingDirection = this->currentMovingDirection;
	Identifier = inIdentifier;

	isVisible = true;

	SubscribeToEvent(EventType::ControllerMoveEvent); // Player responds to move commands
	SubscribeToEvent(EventType::SettingsReloaded); // We can reload the player's settings dynamically
	SubscribeToEvent(EventType::Fire); // Player response to fire command

	// Should the player log everything its doing to log file?
	verbose = false;

	// How long is 1 movement
	moveDurationMs = SettingsManager::Get()->GetInt("player", "moveDurationMs");
}

Player::Player(std::shared_ptr<Room> playerRoom, int playerWidth, int playerHeight, std::string identifier)
	:DrawableGameObject(playerRoom->GetCenter(playerWidth, playerHeight), true), GameObjectsPtr(SceneManager::Get()->GetGameWorld().GetGameObjects())
{
	CommonInit(playerWidth, playerHeight, identifier);

	SetPlayerRoom(playerRoom->GetRoomNumber());
	CenterPlayerInRoom(playerRoom);
}

ListOfEvents Player::HandleEvent(const shared_ptr<Event> event)
{
	ListOfEvents createdEvents;

	BaseProcessEvent(event, createdEvents);
	
	switch (event->type)
	{		
		case EventType::ControllerMoveEvent: // We got a controller movement
			return OnControllerMove(event, createdEvents);
			break;	
		case EventType::Fire: // The user pressed the fire command.
			LogMessage("Fire!", verbose);
			Fire();
			break;
		case EventType::SettingsReloaded: // The system wan't us to re-load our settings
			LogMessage("Reloading player settings", verbose);
			LoadSettings();
			break;
		case EventType::InvalidMove: // The system says that an invalid move was made
			LogMessage("Invalid move", verbose);
			break;
	}

	return createdEvents;
}


const ListOfEvents& Player::OnControllerMove(const shared_ptr<Event>& event, ListOfEvents& createdEvents)
{
	auto controllerMoveEvent = dynamic_pointer_cast<ControllerMoveEvent>(event);	
	
	AddToPlayerMovements(controllerMoveEvent, createdEvents); // NB: movements will be processed udirng updates

	return createdEvents;
}

void Player::AddToPlayerMovements(std::shared_ptr<gamelib::ControllerMoveEvent>& controllerMoveEvent, ListOfEvents& createdEvents)
{
	SetPlayerDirection(controllerMoveEvent->Direction);

	auto doesPlayerHaveMoves = PlayerHasPendingMoves();
	auto lastDirectionIsSameAsCurrent = doesPlayerHaveMoves
		? moveQueue.back()->direction == controllerMoveEvent->Direction
		: false;
	
	// Player has moved in a new direction?
	if (!lastDirectionIsSameAsCurrent)
	{
		// Cancel any pending moves in the other direction
		if (doesPlayerHaveMoves)
		{
			if (debugMovement) 
			{
				Logger::Get()->LogThis("Canceling last movement.");
			}

			moveQueue.clear();
		}

		auto movement = std::shared_ptr<Movement>(new Movement(moveDurationMs, controllerMoveEvent->Direction, maxPixelsToMove, debugMovement));
		
		// Add this as a player move, to the list of pending player moves
		moveQueue.push_back(movement);
	}
}

void Player::Update(float deltaMs)
{	
	Move(deltaMs); // Calculate players new position, based on the movements created by the controller	

	Bounds = CalculateBounds(Position.GetX(), Position.GetY()); // Calculate new bounds based on position
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
		
	// Select the key frames relevant for the direction the player is facing
	SetSpriteAnimationFrameGroup();

	// Allow animation, i.e frames to be switched if there are moves pending to animate
	if (PlayerHasPendingMoves())
	{
		sprite->EnableAnimation();
	}
	else
	{
		sprite->DisableAnimation();
	}

	// Allow the sprite to process duration of last frame
	sprite->Update(deltaMs);
}

void Player::Move(float deltaMs)
{
	// Process queued movements
	for (auto& currentMovement : moveQueue)
	{
		if (!currentMovement->IsComplete())
		{
			// calculate the number of pixels that we should move based on the duration that this movement has had
			currentMovement->Update(deltaMs);	
			
			// Actually move player (set player's position) based on the movement
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

// based on the facing direction, select key frame for sprite that matches that direction
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

// without a sprite object, the player has no visible elements
void Player::SetSprite(shared_ptr<AnimatedSprite> sprite)
{
	this->sprite = sprite;
}

void Player::SetMoveStrategy(shared_ptr<IPlayerMoveStrategy> moveStrategy)
{
	this->moveStrategy = moveStrategy;
}

// Remove the wall in front of the player, i.e in the direction the player is facing
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