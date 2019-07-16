#pragma once
#include <string>
using namespace std;
// Meta data about a resource (uid, name, path, type, scene etc)
class Resource
{
public:	
	Resource(){}
	Resource(int uid, string name, string path, string type, int scene);
	int m_uid;
	string m_name;
	string m_path;
	string m_type;
	int m_scene;
	bool m_IsLoaded;
	

	// All resources can load themselves
	virtual void VLoad() = 0;

	// All resources can unload themselves
	virtual void VUnload() = 0;
	virtual ~Resource(){}
};

