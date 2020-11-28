#include "PositionChangeEvent.h"

position_change_event::position_change_event(Direction dir): event(PositionChangeEventType)
{
	direction = dir;
}
