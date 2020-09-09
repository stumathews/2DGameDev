#pragma once
#include "singleton.h"
#include "global_config.h"

template<typename T>
T singleton()
{
	return Singleton<T>::GetInstance().object;
}
