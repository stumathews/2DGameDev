#pragma once
#include <string>
#include <map>
#include "asset.h"
#include "tinyxml2.h"
#include <memory>
#include "event_subscriber.h"
#include "event_manager.h"
#include <iostream>
using namespace std;
using namespace tinyxml2;

class resource_manager final : public event_subscriber
{
		
	void parse_game_resources();
    void store_asset(std::shared_ptr<asset> the_asset);

	std::map<int, std::vector<std::shared_ptr<asset>>> resources_by_scene;   
	std::map<std::string, std::shared_ptr<asset>> resource_by_name;   
	std::map<int,shared_ptr<asset>> resources_by_uuid;

	int resource_count = 0;
	int loaded_resources_count = 0;
	int unloaded_resources_count = 0;
	
    public:
	resource_manager();	
	std::shared_ptr<asset> get_resource_by_name(string name);
	std::shared_ptr<asset> get_resource_by_uuid(int uuid);
	int get_resource_count() const { return resource_count; }
	void load_current_scene_resources(int level);
	vector<shared_ptr<Event>> process_event(std::shared_ptr<Event> evt) override;

    void initialize();
    string get_subscriber_name() override;

};


