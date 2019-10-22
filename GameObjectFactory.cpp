#include "GameObjectFactory.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Sprite.h"
#include  "TypeAliases.h"
#include "GameObject.h"
#include "Room.h"
using namespace tinyxml2;

/*
Create a Game Object Type from an object's xml definition.
*/
shared_ptr<GameObject> GameObjectFactory::BuildGameObject(tinyxml2::XMLElement * sceneObjectXml)
{
	uint red = 0, green = 0, blue = 0;	
	uint x = 0, y = 0;
	bool visible = false, colourKeyEnabled = false;
	shared_ptr<GraphicsResource> resource;
	
	// The game object we are going to construct
	auto emptyGameObject = shared_ptr<GameObject>(NULL);

	// Traverse the object definition
	for(const XMLAttribute* sceneObjAtt = sceneObjectXml->FirstAttribute(); sceneObjAtt; sceneObjAtt = sceneObjAtt->Next()) 
	{
		std::string detailName = sceneObjAtt->Name();
		std::string detailValue = sceneObjAtt->Value();		
		
		// Discover object's information:

		if(detailName == "resourceId") 
		{
			// Get the associated resource details referenced by this object
			auto meta = ResourceManager::GetInstance().GetResourceByUuid(atoi(detailValue.c_str()));
			if(meta != NULL)
			{
				if(!meta->m_type._Equal("graphic"))
				{ 
					throw new exception(("Cannot load non graphic resource: " + meta->m_name + " type=" + meta->m_type).c_str()); 
				}

				if(meta == NULL) 
				{
					throw new exception(("Could not load resource meta data for resource id:" + detailValue).c_str());
				}

				resource = std::dynamic_pointer_cast<GraphicsResource>(meta);
			}
		}

		// Object's initial x position
		if(detailName._Equal("posx")) 
		{
			x = atoi(detailValue.c_str());
			continue;
		}

		// Object iintial visibility setting
		if(detailName._Equal("visible")) 
		{
			visible = detailValue._Equal("true") ? true : false;			
		}

		// object initial y position
		if(detailName._Equal("posy")) 
		{
			y = atoi(detailValue.c_str());
			continue;
		}

		// object's associated colour key
		if(detailName._Equal("colourKey")) 
		{
			colourKeyEnabled = detailValue._Equal("true") ? true : false;			
			continue;
		}

		// Objects colour
		if(detailName._Equal("r")) {
			red = atof(detailValue.c_str());
			continue;
		}
		if(detailName._Equal("g")) {
			green = atof(detailValue.c_str());
			continue;
		}
		if(detailName._Equal("b")) {
			blue = atof(detailValue.c_str());
			continue;
		}		
	}
		
	// Popuate it with what we found in the object xml definition
	return InitGameObject(emptyGameObject, x, y, resource, colourKeyEnabled, visible, red, green, blue);		
}

std::shared_ptr<GameObject>& GameObjectFactory::InitGameObject(std::shared_ptr<GameObject>& gameObject, uint x, uint y, std::shared_ptr<GraphicsResource>& resource, bool colourKeyEnabled, bool visible, const uint& red, const uint& green, const uint& blue)
{	
	if(resource == NULL)
	{ 
		auto width = 50;
		gameObject = shared_ptr<GameObject>(new Room(x, y, width));
		gameObject->m_Visible = true;
		return gameObject;		
	}

	if( (red < 0 || red > 255)  || (blue < 0 || blue > 255) || (green < 0 || green > 255) )
	{
		throw new exception("Invalid colour values when constructing game object");
	}

	if( x < 0 || y < 0)
	{
		throw new exception("Position values are invalid when constructing a game object");
	}


	// Special animated game object?	
	if(resource->m_bIsAnimated)
	{
		auto framesPerRow = 3, framesPerColumn = 3;
		auto sprite = new Sprite(x, y, 100, resource->m_NumKeyFrames, framesPerRow, framesPerColumn, resource->m_KeyFrameWidth, resource->m_KeyFrameHeight);
		gameObject = shared_ptr<Sprite>(sprite);
		
	} 
	else
	{
		// Normal 2d Sprite (no animation)
		gameObject = shared_ptr<GameObject>(new Sprite(x, y));	
	}
	gameObject->SetGraphicsResource(resource);
	gameObject->m_ColourKeyEnabled = colourKeyEnabled;
	gameObject->m_Visible = visible;

	if (gameObject->m_ColourKeyEnabled)
		gameObject->SetColourKey(red, green, blue);


	if (resource->m_bIsAnimated)
		std::dynamic_pointer_cast<Sprite>(gameObject)->play();
	return gameObject;
}
