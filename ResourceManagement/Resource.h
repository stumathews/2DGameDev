#pragma once
#include <string>
using namespace std;
// Meta data about a resource (uid, name, path, type, level etc)
class Resource
{
public:	
	Resource(int uid, string name, string path, string type, int level);
	int m_uid;
	string m_name;
	string m_path;
	string m_type;
	int m_level;
	

	// All resources can load themselves
	virtual void VLoad() = 0;

	// All resources can unload themselves
	virtual void VUnload() = 0;
	virtual ~Resource(){}
};

