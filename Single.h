#pragma once
template<typename T>
T Single()
{
	return Singleton<T>::GetInstance().object;
}
