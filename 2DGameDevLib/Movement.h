#pragma once
#include "pch.h"
#include "MoveStrategy.h"
#include <memory>

class Room;

class Movement
{
public:
	Movement(float durationMs, std::shared_ptr<Room> towardsRoom, int maxPixels = 20, bool debug = false);
	~Movement();
	bool IsComplete();
	void Update(float deltaMs);
	float TakePixelsToMove();
	float GetPixelsTraveled();
	float GetPixelsStillToTravel();
	void SpendPixels();
	std::shared_ptr<Room> GetTargetRoom();
	static int id;
private:
	float pixelsPerMs;
	float pixelsTraveled;
	float durationMs;
	float pixelsToMove;
	float totalTargetMovePixels;
	bool isComplete;
	std::shared_ptr<Room> towardsRoom;
	bool debug;

};

