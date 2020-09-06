#pragma once
#include <string>
using namespace std;
// Meta data about a resource (uid, name, path, type, scene etc)
class asset
{
public:	
	
	asset(int uid, string name, string path, string type, int scene);
	int m_uid;
	string m_name;
	string m_path;
	string m_type;
	int m_scene;
	bool m_IsLoaded;
	

	// All resources can load themselves
	virtual void load() = 0;

	// All resources can unload themselves
	virtual void unload() = 0;
	virtual ~asset();
};

