#pragma once
#include <string>
#include <map>
#include "asset.h"
#include "tinyxml2.h"
#include <memory>
#include "tinyxml2.h"
#include "GraphicsManager.h"
#include "EventSubscriber.h"
#include "EventManager.h"
#include "Events.h"
#include <iostream>
using namespace std;
using namespace tinyxml2;


class resource_manager final : event_subscriber
{
    public:
        static resource_manager& get()
        {
            static resource_manager instance;			
            return instance;
        }

		std::shared_ptr<asset> get_resource_by_name(string name);
		std::shared_ptr<asset> get_resource_by_uuid(int uuid);
		int get_resource_count() const { return resource_count_; }
		void load_current_scene_resources(int level);
		vector<shared_ptr<Event>> process_event(std::shared_ptr<Event> evt) override;

        void initialize();
				
    private:
		resource_manager();		
		void parse_game_resources();
        void store_resource(std::shared_ptr<asset> resource);
    public:
        string get_subscriber_name() override;
    private:
		std::map<int, std::vector<std::shared_ptr<asset>>> m_ResourcesByLevel;   
		std::map<std::string, std::shared_ptr<asset>> m_ResourcesByName;   
		std::map<int,shared_ptr<asset>> m_ResourcesByUuid;

		int current_level_num_ = 0;
		int resource_count_;
		int loaded_resources_count_ = 0;
		int unloaded_resources_count_ = 0;
};


