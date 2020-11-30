#pragma once
#include <string>
using namespace std;
class Component
{
public:
	Component(string name) : m_Name(name){}
	string m_Name;
	string GetName() { return m_Name; }
	void SetName(string name) { this->m_Name = name;}
};

