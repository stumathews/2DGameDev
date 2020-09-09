#pragma once

#include <SDL.h>
#include "Event.h"
#include "GraphicsManager.h"
#include "EventSubscriber.h"
#include "EventManager.h"
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
	void raise_event(const Event& event);
	void raise_event(const shared_ptr<Event>& the_event);
	shared_ptr<GraphicsResource> get_resource() const;
	void set_graphic_resource(shared_ptr<GraphicsResource> graphic_resource);

	void virtual draw(SDL_Renderer* renderer) = 0;	
	void virtual update();
	virtual void move_up();
	virtual void move_down();
	virtual void move_left();
	virtual void move_right();
	
	vector<shared_ptr<Event>> process_event(const std::shared_ptr<Event> event) override;  // NOLINT(readability-inconsistent-declaration-parameter-name)
	void DetectSideCollision();
	void set_color_key(float r, float g, float b);
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
	shared_ptr<GraphicsResource> graphic_resource; // can be shared by other actors
	map<string, shared_ptr<Component>> components;
	SDL_Color color_key = {};
	int move_interval = global_config::move_interval; // move by intervals of 10 pixels
	void setup_default_subscriptions();
};



