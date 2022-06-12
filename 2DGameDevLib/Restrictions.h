#pragma once
#include <memory>
class Room;

class Restrictions
{
public:
	std::shared_ptr<Room> TopRoom = nullptr;
	std::shared_ptr<Room> RightRoom = nullptr;
	std::shared_ptr<Room> BottomRoom = nullptr;
	std::shared_ptr<Room> LeftRoom = nullptr;

	
	bool IsMovingRight;
	bool IsMovingDown;
	bool IsMovingUp;
	bool IsMovingLeft;
	bool IsMoving;

	bool CanMoveRight;
	bool CanMoveLeft;
	bool CanMoveDown;
	bool CanMoveUp;
};

