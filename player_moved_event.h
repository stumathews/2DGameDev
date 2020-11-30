#pragma once
#include "Event.h"
#include "player_component.h"
#include <memory>
#include <utility>
#include "Player.h"
class player_moved_event : public event
{
public:
	player_moved_event(shared_ptr<player_component> &component) : event(event_type::PlayerMovedEventType), player(component) { }
	shared_ptr<player_component> get_player_component() const { return player; }
	string to_str() override;
private:
	shared_ptr<player_component> player;
};

