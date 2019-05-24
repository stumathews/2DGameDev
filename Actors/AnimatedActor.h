#pragma once
#include "Actor.h"
#include <windows.h>

class AnimatedActor : public Actor
{
public:
	AnimatedActor();
	~AnimatedActor();

private:
	DWORD m_TimeLastFrame;

};

