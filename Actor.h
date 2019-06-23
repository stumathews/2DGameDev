#pragma once
#include <SDL.h>
#include "EventSubscriber.h"
#include "GraphicsResource.h"


// Drawable object
class Actor : public EventSubscriber
{
public:
	Actor(){}
	Actor(int x, int y, int w, int h, int posX, int posY) : posX(posX), posY(posY)
	{
		mBounds = { x = x, y = y, w = w, h = h};	
	}

	virtual void MoveUp()
	{
		posY -= moveInterval;
	}
	virtual void MoveDown()
	{
		posY += moveInterval;
	}
	virtual void MoveLeft()
	{
		posX -= moveInterval;
	}
	virtual void MoveRight()
	{
		posX += moveInterval;
	}

	// Actor can look after itself
	virtual void VDoLogic() = 0;

	// Actor can draw itself, usually refering to the GraphicsResource assoaicted with the actor
	virtual void VDraw(SDL_Renderer* renderer) = 0;
	virtual ~Actor();

	// Set the graphics resource that underlies this Actor
	// There could be many Actors refering to this graphics resource in the game
	void SetGraphicsResource(shared_ptr<GraphicsResource> graphicsResource)
	{
		m_GraphicsResource = graphicsResource;
	}
	bool m_Visible;
	bool m_ColourKeyEnabled = false;
	void SetColourKey(float r, float g, float b)
	{
		m_ColorKey.r = r;
		m_ColorKey.g = g;
		m_ColorKey.b = b;
	}
	int posX;
	int posY;
	shared_ptr<GraphicsResource> GetResource() { return m_GraphicsResource;}
protected:
	// Graphics resource associated with the actor, usually used during the Draw()ing peration when the Actor wishes to draw itself
	// usually in response to the something asking it to draw itself like the GraphicsManager
	shared_ptr<GraphicsResource> m_GraphicsResource; // can be shared by other actors
	SDL_Rect mBounds;
	
	
	SDL_Color m_ColorKey;
	int moveInterval = 50; // move by intervals of 10 pixels

};

