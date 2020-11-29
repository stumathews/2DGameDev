#include "game_object.h"
#include "Events.h"
#include <memory>
#include "Event.h"
#include <iostream>
#include "global_config.h"
#include "constants.h"
#include "resource_manager.h"
#include <SDL_mixer.h>
#include "AudioResource.h"

extern shared_ptr<event_manager> event_admin;
extern shared_ptr<global_config> config;
extern shared_ptr<resource_manager> resource_admin;

void game_object::setup_default_subscriptions()
{
	//event_admin->subscribe_to_event(PositionChangeEventType, this);	
	//event_admin->subscribe_to_event(DoLogicUpdateEventType, this);	
}

vector<shared_ptr<event>> game_object::process_event(const std::shared_ptr<event> the_event)
{
	// Change the object's position
	if(the_event->type == event_type::PositionChangeEventType)
	{
		const auto event = std::dynamic_pointer_cast<position_change_event>(the_event);
		if(event->direction == Up && supports_move_logic)					
			move_up();			
		
		if(event->direction == Down && supports_move_logic)
			move_down();			
		
		if(event->direction == Left && supports_move_logic)
			move_left();			
		
		if(event->direction == Right && supports_move_logic)
			move_right();		
	}

	if(the_event->type == event_type::DoLogicUpdateEventType)
		update();
	return vector<shared_ptr<event>>();
}

void game_object::subscribe_to_event(event_type type)
{
	event_admin->subscribe_to_event(type, this);
}

void game_object::raise_event(const event& the_event)
{
//	event_admin->raise_event(make_unique<event>(the_event), this);
}

void game_object::raise_event(const shared_ptr<event>& the_event)
{
	event_admin->raise_event(the_event, this);
}

shared_ptr<graphic_asset> game_object::get_graphic_asset() const
{
	return graphic_resource;
}

void game_object::draw(SDL_Renderer * renderer)
{
	if(!is_visible)
		return;
	
	draw_resource(renderer);
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
		if (x == 0) 
		{
			is_traveling_left = false;
			Mix_PlayChannel(-1, static_pointer_cast<audio_resource>(resource_admin->get_resource_by_name("high.wav"))->as_fx(), 0);
		}
	}
	else
	{
		if (x == sdl_graphics_manager::get().get_screen_width())
		{
			Mix_PlayChannel(-1, static_pointer_cast<audio_resource>(resource_admin->get_resource_by_name("low.wav"))->as_fx(), 0);
			is_traveling_left = true;
		}
	}
}

game_object::game_object(bool is_visible): event_subscriber(), supports_move_logic(true), is_visible(is_visible), is_color_key_enabled(false), x(0), y(0), is_traveling_left(false), red(0x00), blue(0xFF), green(0x00)
{
	setup_default_subscriptions();
}

game_object::game_object(const int x, const int y, bool is_visible): event_subscriber(), supports_move_logic(false), is_visible(is_visible), x(x), y(y)
{
	red = 0x00;
	blue = 0xFF;
	green = 0x00;
	is_traveling_left = false;
	is_visible = true;
	is_color_key_enabled = false;
	setup_default_subscriptions();
}


void game_object::set_color_key(const Uint8 r, const Uint8 g, const Uint8 b)
{
	color_key.r = r;
	color_key.g = g;
	color_key.b = b;
}

void game_object::add_component(const shared_ptr<Component>& component)
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

void game_object::set_tag(const string tag)
{
	this->tag = tag;
}

string game_object::get_subscriber_name()
{
	return "game_object";
}


void game_object::draw_resource(SDL_Renderer* renderer) const
{
	const auto resource = get_graphic_asset();
	if(resource != nullptr && resource->type == "graphic")
	{
		SDL_Rect draw_location = { x, y, 100,100 };
		const auto rect = get_graphic_asset()->m_bIsAnimated
						?  &graphic_resource->m_viewPort
						: nullptr;
		SDL_RenderCopy( sdl_graphics_manager::get().window_renderer, graphic_resource->texture, rect, &draw_location );
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

void game_object::set_graphic_resource(shared_ptr<graphic_asset> resource)
{
	this->graphic_resource = resource;
}
