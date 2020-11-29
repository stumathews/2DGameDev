#pragma once

#include <SDL.h>
#include "Event.h"
#include "sdl_graphics_manager.h"
#include "event_subscriber.h"
#include "event_manager.h"
#include "Component.h"
#include <map>
#include "global_config.h"


class game_object : public event_subscriber
{
public:
	bool supports_move_logic;
	bool is_visible;
	bool is_color_key_enabled;
	int x;
	int y;

	game_object(bool is_visible = true);
	game_object(int x, int y, bool is_visible = true);

	void subscribe_to_event(event_type type);
	void raise_event(const event& event);
	void raise_event(const shared_ptr<event>& the_event);
	shared_ptr<graphic_asset> get_graphic_asset() const;
	void set_graphic_resource(shared_ptr<graphic_asset> graphic_resource);

	void virtual draw(SDL_Renderer* renderer) = 0;	
	void virtual update() = 0;
	virtual void move_up();
	virtual void move_down();
	virtual void move_left();
	virtual void move_right();
	
	vector<shared_ptr<event>> process_event(const std::shared_ptr<event> event) override;  // NOLINT(readability-inconsistent-declaration-parameter-name)
	void DetectSideCollision();
	void set_color_key(Uint8 r, Uint8 g, Uint8 b);
	void add_component(const shared_ptr<Component>& component);
	bool is_player();
	shared_ptr<Component> find_component(string name);
	bool has_component(string name);
	string get_tag() const;
	void set_tag(string tag);
	string get_subscriber_name() override;
	void draw_resource(SDL_Renderer* renderer) const;
	bool is_resource_loaded() const;

private:
	string tag;
	bool is_traveling_left;
	int red, blue, green;
	shared_ptr<graphic_asset> graphic_resource; // can be shared by other actors
	map<string, shared_ptr<Component>> components;
	SDL_Color color_key = {};
	int move_interval = global_config::move_interval; // move by intervals of 10 pixels

};



