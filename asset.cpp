
#include "asset.h"

/*  A resource's metadata */
asset::asset(int uid, string name, string path, string type, int scene)
	: m_uid(uid), m_path(path), m_type(type), m_name(name), m_scene(scene), m_IsLoaded(false)
{
}

asset::~asset()
= default;

