#pragma once
#include <iostream>
#include <SDL.h>
#include "GameObject.h"
#include <stdbool.h>
#include "RectDetails.h"

class Square : public GameObject 
{
private: 
    int width;
protected:
	bool walls[4];
	RectDetails* rectDetails;
	RectDetails* GetRectDetails(){ return rectDetails; };
	int fill;
	SDL_Rect playerBounds;
	SDL_Rect myBounds;
public: 
	Square(int m_xPos, int m_yPos, int rw, bool fill = false, bool supportMoveLogic = true);    
    
    inline int GetX() { return this->m_xPos; }
	inline int GetY() { return this->m_yPos; }
	inline int GetW() { return width; }
	inline int GetH() { return width; }

	bool IsWalled(int wall) { return walls[wall-1]; }	
	void show(SDL_Renderer* renderer);    
	inline void removeWall(int wall) { this->walls[wall-1] = false; }

	virtual vector<shared_ptr<Event>> ProcessEvent(std::shared_ptr<Event> event);
    virtual void VDraw(SDL_Renderer* renderer) override { show(renderer); };
	inline virtual ~Square() { delete rectDetails;	}
	virtual void VDoLogic(){}
};
