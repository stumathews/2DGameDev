#pragma once

#include <SDL.h>
#include "Drawing.h"
#include <iostream>
#include <SDL_mixer.h>
#include "Events.h"
#include "Event.h"
#include "GraphicsManager.h"
#include "EventSubscriber.h"
#include "EventManager.h"
#include "Component.h"
#include <map>
#include "constants.h"

//extern Mix_Chunk *gScratch;
//extern Mix_Chunk *gHigh;
//extern Mix_Chunk *gMedium;
//extern Mix_Chunk *gLow;

/*
	A game Object 
*/
class GameObject : public IEventSubscriber
{
public:
	GameObject(): 
		red(0x00), 
		blue(0xFF), 
		green(0x00), 
		isTravelingLeft(false), 
		m_Visible(true), 
		m_ColourKeyEnabled(false), 
		m_xPos(0), 
		m_yPos(0), m_DoMoveLogic(true) {}

	GameObject(int m_xPos, int m_yPos): 
		m_xPos(m_xPos),
		m_yPos(m_yPos)
	{
		red = 0x00;
		blue = 0xFF;
		green = 0x00;
		isTravelingLeft = false; // its traveling right	
		m_Visible = true;
		m_ColourKeyEnabled = false;
	}

	bool m_DoMoveLogic = true;
	bool m_Visible;
	bool m_ColourKeyEnabled = false;
	int m_xPos;
	int m_yPos;

	void SubScribeToEvent(EventType type) 
	{ 
		EventManager::GetInstance().SubscribeToEvent(type, this);
	}

	void RaiseEvent(Event event)
	{		
		EventManager::GetInstance().RegisterEvent(std::shared_ptr<Event>(&event));
	}

	void RaiseEvent(shared_ptr<Event> event)
	{		
		EventManager::GetInstance().RegisterEvent(event);
	}

	shared_ptr<GraphicsResource> GetResource() { return m_GraphicsResource; }
	void SetGraphicsResource(shared_ptr<GraphicsResource> graphicsResource)
	{
		m_GraphicsResource = graphicsResource;
	}

	// A game object should know how to draw itself
	void virtual VDraw(SDL_Renderer* renderer) = 0;	
	void virtual VDoLogic() = 0;
	virtual void MoveUp() { m_yPos -= moveInterval; }
	virtual void MoveDown() { m_yPos += moveInterval; }
	virtual void MoveLeft() { m_xPos -= moveInterval; }
	virtual void MoveRight() { m_xPos += moveInterval; }
	virtual void DrawResource(SDL_Renderer* renderer);	

	vector<shared_ptr<Event>> ProcessEvent(const std::shared_ptr<Event> event);
	void DetectSideColission();
	virtual ~GameObject() { }
	
	void SetColourKey(float r, float g, float b)
	{
		m_ColorKey.r = r;
		m_ColorKey.g = g;
		m_ColorKey.b = b;
	}

	void AddComponent(shared_ptr<Component> component)
	{		
		m_Components[component->GetName()] = component;
	}

	bool isPlayer() { return FindComponent(constants::playerComponentName) != NULL; }

	shared_ptr<Component> FindComponent(string name)
	{
		return m_Components[name];
	}
	string GetTag() { return this->Tag;}
	void SetTag(string tag) { this->Tag = tag;}
private:
	string Tag;
	bool isTravelingLeft;
	int red, blue, green;
	shared_ptr<GraphicsResource> m_GraphicsResource; // can be shared by other actors
	map<string, shared_ptr<Component>> m_Components;
	SDL_Rect mBounds = {};	
	SDL_Color m_ColorKey = {};
	int moveInterval = 5; // move by intervals of 10 pixels
};

