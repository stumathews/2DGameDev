#include "Square.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include "PlayerMovedEvent.h"
#include "RectDebugging.h"

using namespace std;

Square::Square(int x, int y, int rw, bool fill, bool supportMoveLogic) 
	: GameObject(m_xPos, m_yPos), 
		fill(fill), 
		width(rw), playerBounds({}), myBounds({x, y, rw, rw})
{ 
  this->rectDetails = new RectDetails(x, y, rw, rw);  
  this->m_DoMoveLogic = supportMoveLogic;   
  walls[0] = true;
  walls[1] = true;
  walls[2] = true;
  walls[3] = true;  
}

void Square::show(SDL_Renderer* renderer) 
{ 
  auto rect = GetRectDetails();
  int ax = rect->getAx();
  int ay = rect->getAy();
  int bx = rect->getBx();
  int by = rect->getBy();
  int cx = rect->getCx();
  int cy = rect->getCy();
  int dx = rect->getDx();
  int dy = rect->getDy();
  
  auto haveTopWall = this->walls[0];
  auto haveRightWall = this->walls[1];
  auto haveBottomWall = this->walls[2];
  auto haveLeftWall = this->walls[3];

  if (haveTopWall) 
	SDL_RenderDrawLine(renderer, ax,ay,bx,by);    
  if (haveRightWall) 
    SDL_RenderDrawLine(renderer, bx,by,cx,cy);	
  if (haveBottomWall) 
    SDL_RenderDrawLine(renderer, cx,cy,dx,dy);
  if (haveLeftWall) 
    SDL_RenderDrawLine(renderer, dx,dy,ax,ay);
   
  SDL_Rect _unused;

  
  
 /* if(fill)
  	 SDL_RenderFillRect(renderer, &myBounds);*/
    
  if(Singleton<GlobalConfig>::GetInstance().object.printDebuggingText)
	  RectDebugging::printInRect(renderer, GetTag().c_str(), &myBounds); 
}

vector<shared_ptr<Event>> Square::ProcessEvent(const std::shared_ptr<Event> event)
{	
	auto newEvents(GameObject::ProcessEvent(event));  // Moves the square, if its set to to movable

	if(event->m_eventType == PlayerMovedEventType)
	{	
		auto playerMovedEvent = std::static_pointer_cast<PlayerMovedEvent>(event);
		
		playerBounds = 	{ 
			playerMovedEvent->GetPlayerComponent()->x, 
			playerMovedEvent->GetPlayerComponent()->y, 
			playerMovedEvent->GetPlayerComponent()->w, 	
			playerMovedEvent->GetPlayerComponent()->h 
		};
	}

	if(event->m_eventType == PositionChangeEventType)
	{
		rectDetails->Init(GetX(), GetY(), GetW(), GetH());
		myBounds = { GetX(), GetY(), GetW(), GetH() };
	}
	
	return newEvents;
}






