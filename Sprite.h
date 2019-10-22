#pragma once
#include "GameObject.h"
#include "TypeAliases.h"


class Sprite : public GameObject
{
private:
	unsigned long m_TimeLastFrame;
public:
	Sprite(uint xPos, uint yPos, uint speed = 0, uint totalFrames = 1, uint framesPerRow = 1, uint framesPerColumn = 1, uint frameWidth = 0, uint frame = 0) : 
		m_Speed(100), m_TotalFrames(totalFrames), m_FramesPerRow(framesPerRow), m_FramesPerColumn(framesPerColumn),
		m_CurrentFrame(0), m_FrameHeight(64), m_FrameWidth(64), m_StartFrame(0), m_TimeLastFrame(0), GameObject(xPos, yPos) { }
	
	uint m_TotalFrames;
	uint m_FramesPerRow;
	uint m_FramesPerColumn;
	uint m_CurrentFrame;
	uint m_StartFrame;
	float m_Speed;
	uint m_FrameWidth;
	uint m_FrameHeight;
	

	virtual void VDraw(SDL_Renderer* renderer)
	{
		update();
		GameObject::VDraw(renderer);
	}

	virtual void VDoLogic()
	{
		GameObject::DetectSideColission();
	}

	void update();
	void play();
	void stop();
	void setFrameRect(uint FrameNumber);
	~Sprite(){};
	bool stopped = false;
};

