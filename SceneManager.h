#pragma once
#include "Layer.h"
#include <list>
#include "EventSubscriber.h"
#include <iostream>

class scene_manager final : public event_subscriber
{

public:

	scene_manager();
	scene_manager(scene_manager const&);	
    void operator=(scene_manager const&)  = delete;	
	bool load_scene_file(const std::string& filename);		
	shared_ptr<Layer> add_layer(const std::string& name);
	const shared_ptr<Layer> find_layer(const std::string& name);
	void remove_layer(const std::string& name);
	void sort_layers();
	static void update() {}
	vector<shared_ptr<Event>> process_event(std::shared_ptr<Event> evt) override;		
	bool is_initialized = false;
    string get_subscriber_name() override;
	std::list<shared_ptr<Layer>> layers;
};

