#include "pch.h"
#include "Movement.h"
#include <memory>
#include "room.h"
#include <common/Logger.h>
#include <sstream>
#include <util/SettingsManager.h>


int Movement::id = 0;

Movement::Movement(float durationMs, std::shared_ptr<Room> towardsRoom, int maxPixels, bool debug)
{
	id++;
	this->durationMs = durationMs;
	this->totalTargetMovePixels = maxPixels;
	this->pixelsPerMs = totalTargetMovePixels / durationMs;
	this->pixelsTraveled = 0;
	this->pixelsToMove = 0;
	this->isComplete = false;
	this->towardsRoom = towardsRoom;
	this->debug = debug;
}

Movement::~Movement()
{
	
}

bool Movement::IsComplete()
{
	return isComplete;
}

void Movement::Update(float deltaMs)
{
	
	if(pixelsTraveled < totalTargetMovePixels)
	{
		pixelsToMove = ceil(pixelsPerMs * deltaMs);
	}
	else
	{
		isComplete = true;
	}

	if(debug)
	{
		std::stringstream message;
		message << id << ": " << "Move " << pixelsToMove << " towards room " << towardsRoom->GetRoomNumber() << ". " << GetPixelsTraveled() << "/" << totalTargetMovePixels;
			
		gamelib::Logger::Get()->LogThis(message.str());
	}
}

float Movement::TakePixelsToMove()
{	
	SpendPixels();
	return pixelsToMove;
}

float Movement::GetPixelsTraveled()
{	
	return pixelsTraveled;
}

float Movement::GetPixelsStillToTravel()
{
	return totalTargetMovePixels - pixelsTraveled;
}

void Movement::SpendPixels()
{
	pixelsTraveled += pixelsToMove;
}

std::shared_ptr<Room> Movement::GetTargetRoom()
{
	return towardsRoom;
}
