#pragma once
#include "tinyxml2.h"
#include "Layer.h"
#include <list>
#include "EventSubscriber.h"
#include <iostream>

// Scene manager deals with managing the actors that make up a scene.
// Actors are chlidren to layers
class scene_manager final : public event_subscriber
{
public:
	 static scene_manager& get()
        {
            static scene_manager instance;			
            return instance;
        }
		scene_manager(scene_manager const&)  = delete;
        void operator=(scene_manager const&)  = delete;
		std::list<shared_ptr<Layer>> m_Layers;
		bool load_scene(const std::string& filename);
		
		shared_ptr<Layer> add_layer(std::string name);
		const shared_ptr<Layer> find_layer(const std::string name);
		void remove_layer(std::string name);
		void sort_layers();
		static void update() {}
		vector<shared_ptr<Event>> process_event(std::shared_ptr<Event> evt) override;
		
		void initialize();
		bool is_initialized = false;
protected:
	
		
private:	
		
	scene_manager();

 public:
	 string get_subscriber_name() override;
};

