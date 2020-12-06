#pragma once
#include <list>
#include "Layer.h"
#include "events/event_subscriber.h"

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
	void start_scene(int scene_id);
	list<shared_ptr<layer>> get_scene_layers() const;
private:
	void add_to_scene(const shared_ptr<game_object>& game_object);	
	void load_new_scene(const shared_ptr<event> &the_event);
	bool load_scene_file(const string &filename);		
	void remove_layer(const string &name);
	void sort_layers();
	shared_ptr<layer> add_layer(const string &name);
	shared_ptr<layer> find_layer(const string &name);

	// Event management
	static void update();
	vector<shared_ptr<event>> handle_event(shared_ptr<event> the_event) override;	
    string get_subscriber_name() override;
		
	list<shared_ptr<layer>> layers;
	string current_scene_name = {};
	bool is_initialized = false;	
};

