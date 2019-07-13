#pragma once
#include "Ball.h"

class Sprite : public GameObject
{
private:
	unsigned long m_TimeLastFrame;
public:
	Sprite(int xPos, int yPos, int speed, int totalFrames, int framesPerRow, int framesPerColumn) : GameObject(xPos, yPos){
		m_Speed = speed;
		m_TotalFrames = totalFrames;		
		m_FramesPerRow = framesPerRow;
		m_FramesPerColumn = framesPerColumn; 
	}
	~Sprite(){};
	 unsigned int m_TotalFrames;
	unsigned int m_FramesPerRow;
	unsigned int m_FramesPerColumn;
	unsigned int m_CurrentFrame;
	unsigned int m_StartFrame;
	float m_Speed;
	unsigned int m_FrameWidth;
	unsigned int m_FrameHeight;

	virtual void VDraw(SDL_Renderer* renderer)
	{
		update();
		GameObject::VDraw(renderer);
	}

	void update();
	void play();
	void stop();
	void setFrameRect(unsigned int FrameNumber);


};

