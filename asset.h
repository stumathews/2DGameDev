#pragma once
#include <string>
using namespace std;

/**
 Meta data about a resource (uid, name, path, type, scene etc)
 */
class asset
{
public:	
	
	asset(int uid, string name, string path, string type, int scene);
	asset() = default;
	int uid;
	string name;
	string path;
	string type;
	int scene;
	bool is_loaded;
	
	// All resources can load themselves
	virtual void load() = 0;

	// All resources can unload themselves
	virtual void unload() = 0;
	virtual ~asset() = default;
};

