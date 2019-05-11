#pragma once
#include <string>
using namespace std;
class Resource
{
public:	
	Resource(int uid, string name, string path, string type, int level);
	int m_uid;
	string m_name;
	string m_path;
	string m_type;
	int m_level;
	~Resource();
};

