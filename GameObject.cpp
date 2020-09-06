#include "GameObject.h"
#include "Events.h"
#include <memory>
#include "Event.h"
#include <iostream>
#include "GlobalConfig.h"


vector<shared_ptr<Event>> game_object::process_event(const std::shared_ptr<Event> evt)
{
	// Change the object's position
	if(evt->m_eventType == PositionChangeEventType)
	{
		const auto event = std::dynamic_pointer_cast<PositionChangeEvent>(evt);
		if(event->m_direction == Up && supports_move_logic)					
			move_up();			
		
		if(event->m_direction == Down && supports_move_logic)
			move_down();			
		
		if(event->m_direction == Left && supports_move_logic)
			move_left();			
		
		if(event->m_direction == Right && supports_move_logic)
			move_right();		
	}

	if(evt->m_eventType == DoLogicUpdateEventType)
		update();
	return vector<shared_ptr<Event>>();
}

void game_object::subscribe_to_event(event_type type)
{
	event_manager::get().subscribe_to_event(type, this);
}

void game_object::raise_event(Event event)
{
	event_manager::get().raise_event(make_unique<Event>(event), this);
}

void game_object::raise_event(const shared_ptr<Event> the_event)
{
	event_manager::get().raise_event(the_event, this);
}

shared_ptr<GraphicsResource> game_object::get_resource() const
{
	return graphic_resource;
}

void game_object::draw(SDL_Renderer * renderer)
{
	// Include base drawing functionality
	// On move draw the entire screen again
	SDL_SetRenderDrawColor(sdl_graphics_manager::get().m_Renderer, 0,0,0,0);
	SDL_RenderClear(sdl_graphics_manager::get().m_Renderer);
	
	draw_resource(renderer);
	// Custom drawing afterwards occus here
}

void game_object::update()
{
}

void game_object::move_up()
{
	y -= move_interval;
}

void game_object::move_down()
{
	y += move_interval;
}

void game_object::move_left()
{
	x -= move_interval;
}

void game_object::move_right()
{
	x += move_interval;
}

void game_object::DetectSideCollision()
	{
		if (is_traveling_left)
		{
			if (x == 0) {
				is_traveling_left = false;
				Mix_PlayChannel(-1, Singleton<GlobalConfig>::GetInstance().object.gLow, 0);
			}
		}
		else
		{
			if (x == sdl_graphics_manager::get().GetScreenWidth())
			{
				Mix_PlayChannel(-1, Singleton<GlobalConfig>::GetInstance().object.gLow, 0);
				is_traveling_left = true;
			}
		}
	}

game_object::game_object(): supports_move_logic(true), is_visible(true), is_color_key_enabled(false), x(0), y(0), is_traveling_left(false), red(0x00), blue(0xFF), green(0x00) {}

game_object::game_object(const int x, const int y): supports_move_logic(false), x(x), y(y)
{
	red = 0x00;
	blue = 0xFF;
	green = 0x00;
	is_traveling_left = false;
	is_visible = true;
	is_color_key_enabled = false;
}

game_object::~game_object() = default;

void game_object::set_color_key(const float r, const float g, const float b)
{
	color_key.r = r;
	color_key.g = g;
	color_key.b = b;
}

void game_object::add_component(shared_ptr<Component> component)
{
	components[component->GetName()] = component;
}

bool game_object::is_player()
{
	return find_component(constants::playerComponentName) != nullptr;
}

shared_ptr<Component> game_object::find_component(string name)
{
	return components[name];
}

bool game_object::has_component(string name)
{
	return components.find(name) != components.end();
}

void game_object::set_tag(string tag)
{
	this->tag = tag;
}

string game_object::get_subscriber_name()
{
	return "game_object";
}


void game_object::draw_resource(SDL_Renderer* renderer) const
{
	const auto resource = get_resource();
	if(resource != nullptr && resource->m_type._Equal("graphic") /*Blit only resources to the screen*/)
	{
		SDL_Rect draw_location = { x, y, 100,100 };
		const auto rect = get_resource()->m_bIsAnimated  ?  &graphic_resource->m_viewPort : nullptr;
		SDL_BlitSurface(graphic_resource->m_Surface, rect, sdl_graphics_manager::get().m_WindowSurface, &draw_location);	
	}
}

bool game_object::is_resource_loaded() const
{
	return graphic_resource != nullptr;
}

string game_object::get_tag() const
{
	return this->tag;
}

void game_object::set_graphic_resource(shared_ptr<GraphicsResource> graphic_resource)
{
	this->graphic_resource = graphic_resource;
}
