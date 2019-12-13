#pragma once
#include <SDL.h>
#include "Tuple.h"
class RectDetails
{
public:
	RectDetails(SDL_Rect rect) : RectDetails(rect.x, rect.y, rect.w, rect.h){}
	RectDetails(int x, int y, int w, int h) : x(x), y(y),w(w),h(h){}
	int getAx(){ return this->x; }
	int getAy(){ return this->y; }
	int getBx(){ return getAx()+w;}
	int getBy(){ return getAy();}
	int getCx(){ return getBx();}
	int getCy(){ return getBy()+w;}
	int getDx(){ return getAx();}
	int getDy(){ return getAy()+w;}

	void SetX(int x) { this->x = x;}
	void SetY(int y) { this->y = y;}
	void SetH(int h) { this->h = h;}
	void SetW(int w) {this->w = w; }

	inline void Init(int x, int y, int w, int h) { 	SetX(x); SetY(y); SetW(w);SetH(h); }

	Coordinate<float> GetA1(float intervalOfWOrH) 
	{
		return Coordinate<float>(getAx() + (this->w * intervalOfWOrH), getAy() );
	}
	Coordinate<float> GetB1(float intervalOfWOrH)
	{
		return Coordinate<float>(getBx() - (this->w * intervalOfWOrH), getBy());
	}
	Coordinate<float> GetC1(float intervalOfWOrH)	
	{
		return Coordinate<float>(getCx() - (this->w * intervalOfWOrH), getCy());
	}
	Coordinate<float> GetD1(float intervalOfWOrH)
	{
		return Coordinate<float>(getDx() + (this->w * intervalOfWOrH), getDy());
	}

	Coordinate<float> GetA2(float intervalOfWOrH) 
	{
		return Coordinate<float>(getAx(), getAy() + (this->w * intervalOfWOrH) );
	}
	Coordinate<float> GetB2(float intervalOfWOrH)
	{
		return Coordinate<float>(getBx(), getBy() + (this->w * intervalOfWOrH));
	}
	Coordinate<float> GetC2(float intervalOfWOrH)	
	{
		return Coordinate<float>(getCx(), getCy() - (this->w * intervalOfWOrH));
	}
	Coordinate<float> GetD2(float intervalOfWOrH)
	{
		return Coordinate<float>(getDx(),  getDy() - (this->w * intervalOfWOrH));
	}
	
private:
	int x,y,w,h;

};

