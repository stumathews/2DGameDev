#pragma once
#include "layer.h"
#include <list>
#include "event_subscriber.h"
#include <iostream>
using namespace std;

// Represents the current scene
class scene_manager final : public event_subscriber
{
public:
	scene_manager();	
	scene_manager(const scene_manager &) = default;
	scene_manager(scene_manager &&) = default;
    scene_manager& operator=(scene_manager const&)  = delete;
	scene_manager& operator=(scene_manager &&) = default;
	~scene_manager() = default;

	bool initialize();
	void load_new_scene(std::shared_ptr<event> evt);
	void add_to_scene(std::shared_ptr<game_object> game_object);	
	std::list<shared_ptr<layer>> get_layers() const;
	void start_scene(int scene_id);
private:
	bool load_scene_file(const std::string& filename);	
	shared_ptr<layer> add_layer(const std::string& name);
	shared_ptr<layer> find_layer(const std::string& name);
	void remove_layer(const std::string& name);
	void sort_layers();

	// event management
	static void update() {}
	vector<shared_ptr<event>> process_event(std::shared_ptr<event> evt) override;	
    string get_subscriber_name() override;

	bool is_initialized = false;	
	list<shared_ptr<layer>> layers;
	string current_scene_name = {};
};

