#pragma once
#include <string>
#include <list>
#include <memory>
#include "GameObject.h"
class Layer
{
public:
	Layer();
	bool m_Visible{};
	unsigned int m_ZOrder{};
	float m_PosX{};
	float m_PosY{};
	std::string m_Name;
	std::list<shared_ptr<game_object>> game_objects;
};

inline Layer::Layer()
= default;


