#pragma once
#include <iostream>
#include <SDL.h>
#include "GameObject.h"
#include <stdbool.h>
#include "RectDetails.h"

class Square : public GameObject 
{
private: 
    int width;
protected:
	bool walls[4];
	RectDetails* rect_details_;
	RectDetails* get_rect_details(){ return rect_details_; };
	int fill;
	SDL_Rect player_bounds_;
	SDL_Rect my_bounds_;
public: 
	Square(int m_xPos, int m_yPos, int rw, bool fill = false, bool supportMoveLogic = true);    
    
    inline int get_x() const { return this->m_xPos; }
	inline int get_y() const { return this->m_yPos; }
	inline int get_w() const { return width; }
	inline int get_h() const { return width; }

	bool is_walled(int wall) { return walls[wall-1]; }	
	void show(SDL_Renderer* renderer);    
	inline void removeWall(int wall) { this->walls[wall-1] = false; }

	virtual vector<shared_ptr<Event>> process_event(std::shared_ptr<Event> event);
    virtual void VDraw(SDL_Renderer* renderer) override { show(renderer); };
	inline virtual ~Square() { delete rect_details_;	}
	virtual void VDoLogic(){}
};
