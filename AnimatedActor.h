#pragma once
#include "Actor.h"
#include <windows.h>

class AnimatedActor : public ActorBase
{
public:
	AnimatedActor();
	~AnimatedActor();
	
	void VDraw();

private:

	DWORD m_TimeLastFrame;

};

