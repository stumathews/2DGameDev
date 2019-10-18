#include <iostream>
#include <SDL.h>

class Room {
  private: 
    int x, y, roomWidth;
    bool walls[4];
    bool visited;
  public: 
    Room(int i, int j, int rw);
    void removeWalls(Room &r);
    void removeWall(int wall);
    void show(SDL_Renderer* renderer);    
    void printWalls();
    void visit();
    int getPositionInVector(int size);
    int getX();
    int getY();
    bool isVisited();
};