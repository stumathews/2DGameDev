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
    public:
	resource_manager();	
	shared_ptr<asset> get_resource_by_name(const string& name);
	shared_ptr<asset> get_resource_by_uuid(int uuid);
	int get_resource_count() const { return resource_count; }
	vector<shared_ptr<event>> process_event(shared_ptr<event> evt) override;
	void unload();

	static bool initialize();
    string get_subscriber_name() override;
	void parse_game_resources();
private:
	
    void store_asset(const shared_ptr<asset>& the_asset);
	map<int, vector<shared_ptr<asset>>> resources_by_scene;   
	map<string, shared_ptr<asset>> resource_by_name;   
	map<int,shared_ptr<asset>> resources_by_uuid;

	int resource_count = 0;
	int loaded_resources_count = 0;
	int unloaded_resources_count = 0;
};


