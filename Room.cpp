#include "Room.h"
#include <SDL.h>
#include "PlayerMovedEvent.h"
using namespace std;

Room::Room(int x, int y, int rw, bool fill) : fill(fill)
{
  this->roomWidth = rw;
  this->m_xPos = x;
  this->m_yPos = y;
  
  walls[0] = true;
  walls[1] = true;
  walls[2] = true;
  walls[3] = true;
  visited = false;
}

void Room::show(SDL_Renderer* renderer) 
{  
  int ax = this->m_xPos;
  int ay = this->m_yPos;
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

  if(fill)
  {
	 SDL_Rect rect;
	 rect.x = this->m_xPos;
	 rect.y = this->m_yPos;
	 rect.w = roomWidth;
	 rect.h = roomWidth;
	 
	  SDL_RenderFillRect(renderer, &rect);
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
  return this->m_xPos * size + this->m_yPos;
}

int Room::getX() {
  return this->m_xPos;
}

int Room::getY() {
  return this->m_yPos;
}

bool Room::isVisited() {
  return this->visited;
}


void Room::ProcessEvent(std::shared_ptr<Event> event)
{	
	auto playerComponent = FindComponent("PlayerDetails");
	auto arePlayer = playerComponent != NULL;
	if(arePlayer)
	{
		auto player = static_pointer_cast<PlayerComponent>(playerComponent);
		player->x = getX();
		player->y = getY();
		auto playerMovedEvent = new PlayerMovedEvent(player);
		auto event = shared_ptr<PlayerMovedEvent>(playerMovedEvent);
		RaiseEvent(event);
	}

	if(event->m_eventType == PlayerMovedEventType)
	{
		std::shared_ptr<PlayerMovedEvent> cpe = std::static_pointer_cast<PlayerMovedEvent>(event);
		SDL_Rect me { this->getX(), this->getY(), this->roomWidth, this->roomWidth };
		SDL_Rect player { cpe->GetPlayerComponent()->x, cpe->GetPlayerComponent()->y, cpe->GetPlayerComponent()->w, cpe->GetPlayerComponent()->h };
		SDL_Rect result;
		fill = SDL_IntersectRect(&me, &player, &result);
	}

	GameObject::ProcessEvent(event);
}

void Room::VDraw(SDL_Renderer* renderer)
{	
	show(renderer);
}

void Room::VDoLogic()
{
}



