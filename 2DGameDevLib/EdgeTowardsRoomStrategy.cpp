#include "pch.h"
#include "Player.h"
#include "Room.h"
#include "scene/SceneManager.h"
#include "EdgeTowardsRoomStrategy.h"

EdgeTowardsRoomStrategy::EdgeTowardsRoomStrategy(std::shared_ptr<Player> player, int edgeIncrement)
{
	this->player = player;
	this->edgeIncrement = edgeIncrement;
}

gamelib::coordinate<int> EdgeTowardsRoomStrategy::CalculatePlayerMoveTo(std::shared_ptr<Room> room)
{
	int move_interval = 3;

	auto gameObjects = gamelib::SceneManager::Get()->GetGameWorld().GetGameObjects();
	auto playersRoom = player->GetCurrentRoom(gameObjects);
	auto moveDown = playersRoom->GetNeighbourIndex(Side::Bottom) == room->GetRoomNumber();
	auto moveUp = playersRoom->GetNeighbourIndex(Side::Top) == room->GetRoomNumber();
	auto moveLeft = playersRoom->GetNeighbourIndex(Side::Left) == room->GetRoomNumber();
	auto moveRight = playersRoom->GetNeighbourIndex(Side::Right) == room->GetRoomNumber();
		
	int resulting_x;	
	int resulting_y; 
	
	resulting_y = player->y;
	resulting_x = player->x;

	if(moveDown)
	{
		resulting_y += move_interval;
	}

	if(moveUp)
	{
		resulting_y -= move_interval;
	}

	if(moveLeft)
	{
		resulting_x -= move_interval;
	}

	if(moveRight)
	{
		resulting_x += move_interval;
	}

	return gamelib::coordinate<int>(resulting_x, resulting_y);
}

void EdgeTowardsRoomStrategy::MoveTo(std::shared_ptr<Room> room)
{
	// Edge player towards the room
	auto resultingMove = CalculatePlayerMoveTo(room);

	// Set the player's new position
	player->x = resultingMove.GetX();
	player->y = resultingMove.GetY();	

	// This is required
	player->Update(0.0f);
}


bool EdgeTowardsRoomStrategy::WouldHitInnerBounds(std::shared_ptr<Room>& room)
{
	// Calculate where the player would be if the move is done
	auto mockPlayerPosition = CalculatePlayerMoveTo(room);

	// Calculate the location of the hotspot at that location
	auto mockPlayerHotSpot = player->CalculateHotspot(mockPlayerPosition.GetX(), mockPlayerPosition.GetY());

	// Setup the bounds for the simulated hotspot
	auto mockPlayerHotSpotBounds = SDL_Rect { mockPlayerHotSpot.GetX(), mockPlayerHotSpot.GetY(), 1, 1 };
	
	// Check if the player would intersect with the inner bounds of the room
	SDL_Rect result;
	return SDL_IntersectRect(&room->InnerBounds, &mockPlayerHotSpotBounds, &result);
}

bool EdgeTowardsRoomStrategy::IsTerminalRooms(std::shared_ptr<Room> room1, std::shared_ptr<Room> room2)
{
	return room1->GetRoomNumber() == room2->GetRoomNumber();
}

bool EdgeTowardsRoomStrategy::CanMoveUp(const bool& isMovingUp, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& aboveRoom)
{
	if(!isMovingUp)
		return false;

	auto hasBlockingWalls = aboveRoom->HasBottomWall() && currentRoom->HasTopWall();
	return IsTerminalRooms(aboveRoom, currentRoom) ?  WouldHitInnerBounds(currentRoom) : !hasBlockingWalls || !WouldHitInnerBounds(aboveRoom);
}


bool EdgeTowardsRoomStrategy::CanMoveDown(const bool& isMovingDown, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& bottomRoom)
{
	if(!isMovingDown)
		return false;

	auto hasNoBlockingWalls = !bottomRoom->HasTopWall() && !currentRoom->HasBottomWall();
	return IsTerminalRooms(bottomRoom, currentRoom) 
		? WouldHitInnerBounds(currentRoom) 
		: hasNoBlockingWalls || !WouldHitInnerBounds(bottomRoom);
}

bool EdgeTowardsRoomStrategy::CanMoveLeft(const bool& isMovingLeft, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& leftRoom)
{
	if(!isMovingLeft)
		return false;

	auto hasNoBlockingWalls = !leftRoom->HasRightWall() && !currentRoom->HasLeftWall();
	return IsTerminalRooms(leftRoom, currentRoom) 
		? WouldHitInnerBounds(currentRoom) 
		: hasNoBlockingWalls || !WouldHitInnerBounds(leftRoom);
}

bool EdgeTowardsRoomStrategy::CanMoveRight(const bool& isMovingRight, std::shared_ptr<Room>& currentRoom, std::shared_ptr<Room>& rightRoom)
{
	if(!isMovingRight)
		return false;

	auto hasNoBlockingWalls = !rightRoom->HasLeftWall() && !currentRoom->HasRightWall();
	return IsTerminalRooms(rightRoom, currentRoom) 
		? WouldHitInnerBounds(currentRoom) 
		: hasNoBlockingWalls || !WouldHitInnerBounds(rightRoom);
}
