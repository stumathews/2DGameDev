#pragma once
#include "event_subscriber.h"

class scene_loaded_event : public event
{
public:
	scene_loaded_event(int scene_id);
	string to_str() override;
	int scene_id_;
};
