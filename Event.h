#pragma once
#include <string>
using namespace std;

enum event_type
{
	PositionChangeEventType,
	LevelChangedEventType,
	DoLogicUpdateEventType,
	AddGameObjectToCurrentScene,
	PlayerMovedEventType
};

class event
{
public:
	explicit event(event_type type);
	int event_id = 0;
	bool processed = false;
	event_type type;
	virtual event_type get_type();
};

std::string operator+(const std::string& str, const event_type type);



