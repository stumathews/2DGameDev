#pragma once
#include <SDL.h>
#include "EventSubscriber.h"
#include "GraphicsResource.h"


/* Anything that inherits from GameObjectBase is an game object has state and can draw and update itself 
* Its mean to be derived from as it contains some base functionality shared by all actors such
* as updating its position, Setting its graphic resource.
* VDoLogic and VDraw should be implemented by a derived class
*/

class GameObjectBase : public EventSubscriber
{
public:
		
	bool m_Visible;
	bool m_ColourKeyEnabled = false;
	int m_xPos;
	int m_yPos;

	GameObjectBase(){}
	GameObjectBase(int m_xPos, int m_yPos) : m_xPos(m_xPos), m_yPos(m_yPos) { }

	virtual void MoveUp() { m_yPos -= moveInterval; }
	virtual void MoveDown() { m_yPos += moveInterval; }
	virtual void MoveLeft() { m_xPos -= moveInterval; }
	virtual void MoveRight() { m_xPos += moveInterval; }

	// Actor can look after itself
	virtual void VDoLogic() = 0;

	// Actor can draw itself, usually refering to the GraphicsResource assoaicted with the actor	
	
	virtual void DrawResource(SDL_Renderer* renderer);
	
	virtual ~GameObjectBase();

	// Set the graphics resource that underlies this Actor
	// There could be many Actors refering to this graphics resource in the game
	void SetGraphicsResource(shared_ptr<GraphicsResource> graphicsResource)
	{
		m_GraphicsResource = graphicsResource;
	}
	void SetColourKey(float r, float g, float b)
	{
		m_ColorKey.r = r;
		m_ColorKey.g = g;
		m_ColorKey.b = b;
	}
	
	shared_ptr<GraphicsResource> GetResource() { return m_GraphicsResource;}
protected:
	shared_ptr<GraphicsResource> m_GraphicsResource; // can be shared by other actors
	SDL_Rect mBounds = {};	
	SDL_Color m_ColorKey = {};
	int moveInterval = 5; // move by intervals of 10 pixels
};