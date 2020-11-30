#include "Square.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include "player_moved_event.h"
#include "RectDebugging.h"

using namespace std;

shared_ptr<rect_details> square::get_rect_details() const
{
	return rect_details_;
}

square::square(int x, int y, int rw, bool fill, bool supports_move_logic, bool is_visible) : game_object(x, y), width(rw), fill(fill), player_bounds_({}), my_bounds_({x, y, rw, rw})
{ 
  this->rect_details_ = make_shared<rect_details>(x, y, rw, rw);  
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
}

void square::remove_wall(int wall)
{
	this->walls[wall - 1] = false;
}

string square::get_identifier()
{
	return "square";
}

vector<shared_ptr<event>> square::process_event(const std::shared_ptr<event> event)
{	
	auto new_events(game_object::process_event(event));  // Moves the square, if its set to to movable

	if(event->type == event_type::PlayerMovedEventType)
	{
		const auto moved_event = std::static_pointer_cast<player_moved_event>(event);

		// determine where the player is
		player_bounds_ = 	
		{ 
			moved_event->get_player_component()->x, 
			moved_event->get_player_component()->y, 
			moved_event->get_player_component()->w, 	
			moved_event->get_player_component()->h 
		};
	}

	if(event->type == event_type::PositionChangeEventType)
	{
		rect_details_->init(get_x(), get_y(), get_w(), get_h());
		my_bounds_ = { get_x(), get_y(), get_w(), get_h() };
	}
	
	return new_events;
}

void square::draw(SDL_Renderer* renderer)
{
	// black
	SDL_SetRenderDrawColor(renderer, 0, 0,0,0);
	
	const auto rect = get_rect_details();
	const int ax = rect->get_ax();
	const int ay = rect->get_ay();
	const int bx = rect->get_bx();
	const int by = rect->get_by();
	const int cx = rect->get_cx();
	const int cy = rect->get_cy();
	const int dx = rect->get_dx();
	const int dy = rect->get_dy();

	const auto have_top_wall = this->walls[0];
	const auto have_right_wall = this->walls[1];
	const auto have_bottom_wall = this->walls[2];
	const auto have_left_wall = this->walls[3];

	if (have_top_wall) 
		SDL_RenderDrawLine(renderer, ax,ay,bx,by);
	
	if (have_right_wall) 
		SDL_RenderDrawLine(renderer, bx,by,cx,cy);
	
	if (have_bottom_wall) 
		SDL_RenderDrawLine(renderer, cx,cy,dx,dy);
	
	if (have_left_wall) 
		SDL_RenderDrawLine(renderer, dx,dy,ax,ay);

	SDL_Rect _unused;

	if(fill)
	 SDL_RenderFillRect(renderer, &my_bounds_);

	if(global_config::print_debugging_text)
	  RectDebugging::printInRect(renderer, get_tag().c_str(), &my_bounds_); 
}






