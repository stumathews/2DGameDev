#pragma once
#include "singleton.h"
#include "GlobalConfig.h"

template<typename T>
T singleton()
{
	return Singleton<T>::GetInstance().object;
}
