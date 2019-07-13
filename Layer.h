#pragma once
#include <string>
#include <list>
#include "SceneItem.h"
#include <memory>
#include "Actor.h"
class Layer
{
public:
	Layer();
	~Layer();
	bool m_Visible;
	unsigned int m_ZOrder;
	float m_PosX;
	float m_PosY;
	std::string m_Name;
	std::list<shared_ptr<ActorBase>> m_objects;
};

