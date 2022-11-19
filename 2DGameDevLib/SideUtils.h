#pragma once

class SideUtils
{
public:
	// Turns a side enum into a loggable string
	static const char* SideToString(Side side)
	{
		switch (side)
		{
			case Side::Top: return "Top";
			case Side::Right: return "Right";
			case Side::Bottom: return "Bottom";
			case Side::Left: return "Left";
			default: return "[Unknown Side]";
		}
	}
};