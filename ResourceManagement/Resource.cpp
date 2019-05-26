#include "Resource.h"




Resource::Resource(int uid, string name, string path, string type, int level)
	: m_uid(uid), m_path(path), m_type(type), m_name(name), m_level(level), m_IsLoaded(false)
{
}

