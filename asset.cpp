
#include "asset.h"

/*  A resource's metadata */
asset::asset(int uid, string name, string path, string type, int scene)
	: uid(uid), path(path), type(type), name(name), scene(scene), is_loaded(false)
{
}


