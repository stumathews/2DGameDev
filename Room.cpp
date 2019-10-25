#include "Room.h"
#include <SDL.h>
using namespace std;

Room::Room(int x, int y, int rw) 
{
  this->roomWidth = rw;
  this->x = x;
  this->y = y;
  
  walls[0] = true;
  walls[1] = true;
  walls[2] = true;
  walls[3] = true;
  visited = false;
}

void Room::removeWalls(shared_ptr<Room> &r) {
  if (this->x - r->x == -1) {
    this->removeWall(1);
    r->removeWall(3);
  } 
  if (this->x - r->x == 1) {
    this->removeWall(3);
    r->removeWall(1);
  } 
  if (this->y - r->y == -1) {
    this->removeWall(2);
    r->removeWall(0);
  } 
  if (this->y - r->y == 1) {
      this->removeWall(0);
      r->removeWall(2);
  } 
}

void Room::show(SDL_Renderer* renderer) 
{  
  int ax = this->x;
  int ay = this->y;
  int bx = ax+roomWidth;
  int by = ay;
  int cx = bx;
  int cy = by+roomWidth; // but is it really the height
  int dx = ax;
  int dy = ay+roomWidth;
  if (this->walls[0]) {
    SDL_RenderDrawLine(renderer, ax,ay,bx,by);
  }
  if (this->walls[1]) {
    SDL_RenderDrawLine(renderer, bx,by,cx,cy);
  }
  if (this->walls[2]) {
    SDL_RenderDrawLine(renderer, cx,cy,dx,dy);
  }
  if (this->walls[3]) {
    SDL_RenderDrawLine(renderer, dx,dy,ax,ay);
  }
 
}

void Room::printWalls() {
  for (int i = 0; i < 4; i++) {
    std::cout << walls[i] << " ";
  }
  std::cout << "\n";
}



void Room::removeWall(int w) {
  this->walls[w-1] = false;
}

void Room::visit() {
  this->visited = true;
}

int Room::getPositionInVector(int size) {
  return this->x * size + this->y;
}

int Room::getX() {
  return this->x;
}

int Room::getY() {
  return this->y;
}

bool Room::isVisited() {
  return this->visited;
}

void Room::VDraw(SDL_Renderer* renderer)
{	
	show(renderer);
}

void Room::VDoLogic()
{
}
