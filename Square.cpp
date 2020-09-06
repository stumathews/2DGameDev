#include "Square.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include "PlayerMovedEvent.h"
#include "RectDebugging.h"

using namespace std;

RectDetails* square::get_rect_details() const
{
	return rect_details_;
}

square::square(int x, int y, int rw, bool fill, bool supports_move_logic) : game_object(x, y), width(rw), fill(fill), player_bounds_({}), my_bounds_({x, y, rw, rw})
{ 
  this->rect_details_ = new RectDetails(x, y, rw, rw);  
  this->supports_move_logic = supports_move_logic;   
  walls[0] = true;
  walls[1] = true;
  walls[2] = true;
  walls[3] = true;  
}

int square::get_x() const
{
	return this->x;
}

int square::get_y() const
{
	return this->y;
}

int square::get_w() const
{
	return width;
}

int square::get_h() const
{
	return width;
}

bool square::is_walled(int wall)
{
	return walls[wall - 1];
}

void square::update()
{
}

square::~square()
{
	delete rect_details_;
}

void square::removeWall(int wall)
{
	this->walls[wall - 1] = false;
}

void square::show(SDL_Renderer* renderer) 
{ 
  auto rect = get_rect_details();
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
    
  if(GlobalConfig::print_debugging_text)
	  RectDebugging::printInRect(renderer, get_tag().c_str(), &my_bounds_); 
}

vector<shared_ptr<Event>> square::process_event(const std::shared_ptr<Event> event)
{	
	auto newEvents(game_object::process_event(event));  // Moves the square, if its set to to movable

	if(event->m_eventType == PlayerMovedEventType)
	{	
		auto playerMovedEvent = std::static_pointer_cast<PlayerMovedEvent>(event);
		
		player_bounds_ = 	{ 
			playerMovedEvent->GetPlayerComponent()->x, 
			playerMovedEvent->GetPlayerComponent()->y, 
			playerMovedEvent->GetPlayerComponent()->w, 	
			playerMovedEvent->GetPlayerComponent()->h 
		};
	}

	if(event->m_eventType == PositionChangeEventType)
	{
		rect_details_->Init(get_x(), get_y(), get_w(), get_h());
		my_bounds_ = { get_x(), get_y(), get_w(), get_h() };
	}
	
	return newEvents;
}

void square::draw(SDL_Renderer* renderer)
{
	show(renderer);
}






