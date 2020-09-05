#pragma once
#include "Single.h"
#include "GlobalConfig.h"

template<typename T>
T Single()
{
	return Singleton<T>::GetInstance().object;
}
