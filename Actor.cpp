#include "Actor.h"
#include <iostream>

//
//
//Actor::~Actor()
//{
//}

GameObjectBase::~GameObjectBase()
{
	std::cout << "~Actor()" << std::endl;
}

