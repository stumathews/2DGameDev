#include <iostream>
#include <SDL.h>
#include "GameObject.h"
#include <stdbool.h>

class Room : public GameObject {
  private: 
    int roomWidth;
    bool walls[4];
    bool visited;
  public: 
    Room(int m_xPos, int m_yPos, int rw, bool fill = false);
    void removeWalls(shared_ptr<Room> &r);
    void removeWall(int wall);
    void show(SDL_Renderer* renderer);    
    void printWalls();
    void visit();
    int getPositionInVector(int size);
    int getX();
    int getY();
    bool isVisited();
	bool IsWalled(int wall) { return walls[wall-1];  }
	bool fill;

	// Inherited via GameObject
	virtual void VDraw(SDL_Renderer* renderer) override;
	virtual void VDoLogic() override;
};
