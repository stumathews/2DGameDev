#pragma once
template <class T>
class Coordinate
{
public:
	Coordinate(T x, T y) : x(x), y(y){}
	T GetX(){ return this->x;}
	T GetY(){ return this->y;}
private: 
	T x, y;
};

