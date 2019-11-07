#include "Square.h"
#include <SDL.h>

#include "PlayerMovedEvent.h"
using namespace std;

Square::Square(int x, int y, int rw, bool fill, bool supportMoveLogic) : fill(fill)
{
  Square::playerBounds = {};
  this->topRect = this->bottomRect = this->rightRect = this->leftRect = {0,0,0,0};
  this->roomWidth = rw;
  this->m_xPos = x;
  this->m_yPos = y;
  this->rectDetails = new RectDetails(x, y, rw, rw);
  this->m_DoMoveLogic = supportMoveLogic;
  
  this->touchedTop = this->touchedRight = this->touchedBottom = this->touchedLeft = false;
    
  walls[0] = true;
  walls[1] = true;
  walls[2] = true;
  walls[3] = true;
  visited = false;
}

void Square::show(SDL_Renderer* renderer) 
{  
  int ax = rectDetails->getAx();
  int ay = rectDetails->getAy();
  int bx = rectDetails->getBx();
  int by = rectDetails->getBy();
  int cx = rectDetails->getCx();
  int cy = rectDetails->getCy(); // but is it really the height
  int dx = rectDetails->getDx();
  int dy = rectDetails->getDy();
   

  Uint8 r, g, b, a;
  
	SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);		
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 0);	
	auto a1x = GetRectDetails()->GetA1(0.2f).GetX();
	auto a1y = GetRectDetails()->GetA1(0.2f).GetY();
	auto b1x = GetRectDetails()->GetB1(0.2f).GetX();
	auto b1y = GetRectDetails()->GetB1(0.2f).GetY();
	SDL_RenderDrawLine(renderer, 
		a1x,
		a1y, 
		b1x,
		b1y);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);	
	

  if (this->walls[0]) 
  {		  
	  if(touchedTop)
	  {
		SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);		
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);	
		SDL_RenderDrawLine(renderer, ax,ay,bx,by);
		SDL_SetRenderDrawColor(renderer, r, g, b, a);	
	  }
	  else
	  {	
		SDL_RenderDrawLine(renderer, ax,ay,bx,by);
	  }
    
  }
  if (this->walls[1]) {	
    SDL_RenderDrawLine(renderer, bx,by,cx,cy);
  }
  if (this->walls[2]) {	  
    SDL_RenderDrawLine(renderer, cx,cy,dx,dy);
  }
  if (this->walls[3]) 
  {	  
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

  /* Lets draw the players position within our bounds*/
  

 // cout << "PlayerBounds (" << playerBounds.x << ", " << playerBounds.y << ", " << playerBounds.w << "," << playerBounds.h << ")" << endl;
 
}

void Square::printWalls() {
  for (int i = 0; i < 4; i++) {
    std::cout << walls[i] << " ";
  }
  std::cout << "\n";
}



void Square::removeWall(int w) {
  this->walls[w-1] = false;
}

void Square::visit() {
  this->visited = true;
}

int Square::getPositionInVector(int size) {
  return this->m_xPos * size + this->m_yPos;
}

int Square::GetX() {
  return this->m_xPos;
}

int Square::GetY() {
  return this->m_yPos;
}

bool Square::isVisited() {
  return this->visited;
}


void Square::ProcessEvent(std::shared_ptr<Event> event)
{	
	GameObject::ProcessEvent(event); // Moves the square, if its set to to movable
		
	const float intervalOfWOrh = 1.0f/5.0f;
	topRect = 
		{ 
			/* x: */ (int)GetRectDetails()->GetA1(intervalOfWOrh).GetX(), 
			/* y: */ (int)GetRectDetails()->GetA1(intervalOfWOrh).GetY(), 
			/* w: */ (int)GetRectDetails()->GetB1(intervalOfWOrh).GetX(), 
			/* h: */ 1 
		};

	rightRect = 
		{ 
			/* x: */ (int)GetRectDetails()->GetB2(intervalOfWOrh).GetX(), 
			/* y: */ (int)GetRectDetails()->GetB2(intervalOfWOrh).GetY(), 
			/* w: */ (int)GetRectDetails()->GetC2(intervalOfWOrh).GetY(), 
			/* h: */ 1
		};

	leftRect = 
		{ 
			/* x: */ (int)GetRectDetails()->GetA2(intervalOfWOrh).GetX(), 
			/* y: */ (int)GetRectDetails()->GetA2(intervalOfWOrh).GetY(), 
			/* w: */ (int)GetRectDetails()->GetD2(intervalOfWOrh).GetY(), 
			/* h: */ 1
		};

	bottomRect =
		{ 
			/* x: */ (int)GetRectDetails()->GetC1(intervalOfWOrh).GetX(), 
			/* y: */ (int)GetRectDetails()->GetC1(intervalOfWOrh).GetY(), 
			/* w: */ (int)GetRectDetails()->GetD1(intervalOfWOrh).GetX(), 
			/* h: */ 1 
		};

	/* As a Square/Room, we are interested in knowing where the player is - primarily for collision detection */
	if(event->m_eventType == PlayerMovedEventType)
	{		
		auto playerMovedEvent = std::static_pointer_cast<PlayerMovedEvent>(event);
		
		/* Keep track of ourselves*/
		roomBounds =
		{ 
			this->GetX(), 
			this->GetY(),
			this->roomWidth, 
			this->roomWidth 
		};

		/* Keep track of where the player is for future calculations */
		playerBounds =
		{ 
			playerMovedEvent->GetPlayerComponent()->x, 
			playerMovedEvent->GetPlayerComponent()->y, 
			playerMovedEvent->GetPlayerComponent()->w,
			playerMovedEvent->GetPlayerComponent()->h 
		};
		
		/* Rudimentary collision detection */
		SDL_Rect _unused;
		touchedTop = SDL_IntersectRect(&topRect, &playerBounds, &_unused);		
		touchedRight = SDL_IntersectRect(&rightRect, &playerBounds, &_unused);	
		touchedLeft = SDL_IntersectRect(&leftRect, &playerBounds, &_unused);		
		touchedBottom = SDL_IntersectRect(&bottomRect, &playerBounds, &_unused);		
	}
}

void Square::VDraw(SDL_Renderer* renderer)
{	
	show(renderer);
}

void Square::VDoLogic()
{
}



