#pragma once
#include <iostream>
#include <SDL.h>
#include "GameObject.h"
#include <stdbool.h>
#include "RectDetails.h"

class Square : public GameObject 
{
  private: 
    int roomWidth;
    bool walls[4];
    bool visited;
	RectDetails* rectDetails;
  public: 
    Square(int m_xPos, int m_yPos, int rw, bool fill = false, bool supportMoveLogic = true);  
    void removeWall(int wall);
    void show(SDL_Renderer* renderer);    
    void printWalls();
    void visit();
    int getPositionInVector(int size);
    int GetX();
    int GetY();
	inline int GetW() {return roomWidth;}
	inline int GetH() {return roomWidth;}
    bool isVisited();
	bool IsWalled(int wall) { return walls[wall-1];  }
	SDL_Rect playerBounds;
	SDL_Rect roomBounds;
	
	inline RectDetails* GetRectDetails() 
	{ 
		if(m_DoMoveLogic) // We we support move logic, we'll have to update the rect with the latest values as these have changed with moves
			this->rectDetails->Init(GetX(), GetY(), GetW(), GetH());
		
		return this->rectDetails; 
	}
	bool touchedTop, touchedRight, touchedBottom, touchedLeft;
	SDL_Rect topRect, bottomRect, leftRect, rightRect = {-1,-1,-1,-1};
	
	int fill;

	// Inherited via GameObject
	virtual void ProcessEvent(std::shared_ptr<Event> event);
	virtual void VDraw(SDL_Renderer* renderer) override;
	virtual void VDoLogic() override;
	virtual ~Square()
	{
		delete rectDetails;
	}
};
