#pragma once
#include <string>
#include <map>
#include "../tinyxml2.h"
#include <memory>
#include <iostream>


#include "asset/asset.h"
#include "events/event_subscriber.h"
using namespace std;
using namespace tinyxml2;

class resource_manager final : public event_subscriber
{	
    public:
	resource_manager();
		
	shared_ptr<asset> get(const string& name);
	shared_ptr<asset> get(int uuid);
	int get_resource_count() const { return resource_count; }
	vector<shared_ptr<event>> handle_event(shared_ptr<event> the_event) override;
	void unload();

	bool initialize();
    string get_subscriber_name() override;
	void read_resources();
private:

	void load_level_assets(int level);
    void store_asset(const shared_ptr<asset>& the_asset);
	map<int, vector<shared_ptr<asset>>> resources_by_scene;   
	map<string, shared_ptr<asset>> resource_by_name;   
	map<int,shared_ptr<asset>> resources_by_uuid;

	int resource_count = 0;
	int loaded_resources_count = 0;
	int unloaded_resources_count = 0;
};


