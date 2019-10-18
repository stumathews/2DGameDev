#include "GameObjectFactory.h"
#include "ResourceManager.h"
#include "Ball.h"
#include "Sprite.h"
#include  "TypeAliases.h"
using namespace tinyxml2;

/*
Create a Game Object Type from an object's xml definition.
*/
shared_ptr<GameObjectBase> GameObjectFactory::BuildGameObject(tinyxml2::XMLElement * sceneObjectXml)
{
	uint red = 0, green = 0, blue = 0;	
	uint x = 0, y = 0;
	bool visible = false, colourKeyEnabled = false;
	shared_ptr<GraphicsResource> resource;
	
	// Traverse the object definition
	for(const XMLAttribute* sceneObjAtt = sceneObjectXml->FirstAttribute(); sceneObjAtt; sceneObjAtt = sceneObjAtt->Next()) 
	{
		std::string detail = sceneObjAtt->Name();
		std::string detailValue = sceneObjAtt->Value();		
		
		// Discover object's information:

		if(detail == "resourceId") 
		{
			// Get the associated resource details referenced by this object
			auto meta = ResourceManager::GetInstance().GetResourceByUuid(atoi(detailValue.c_str()));
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

		// Object's initial x position
		if(detail._Equal("posx")) 
		{
			x = atoi(detailValue.c_str());
			continue;
		}

		// Object iintial visibility setting
		if(detail._Equal("visible")) 
		{
			visible = detailValue._Equal("true") ? true : false;			
		}

		// object initial y position
		if(detail._Equal("posy")) 
		{
			y = atoi(detailValue.c_str());
			continue;
		}

		// object's associated colour key
		if(detail._Equal("colourKey")) 
		{
			colourKeyEnabled = detailValue._Equal("true") ? true : false;			
			continue;
		}

		// Objects colour
		if(detail._Equal("r")) {
			red = atof(detailValue.c_str());
			continue;
		}
		if(detail._Equal("g")) {
			green = atof(detailValue.c_str());
			continue;
		}
		if(detail._Equal("b")) {
			blue = atof(detailValue.c_str());
			continue;
		}		
	}
		
	auto gameObject = shared_ptr<GameObjectBase>(NULL);
	
	if(resource->m_bIsAnimated)
	{
		auto framesPerRow = 3, framesPerColumn = 3;
		auto sprite = new Sprite(x, y, 100, resource->m_NumKeyFrames, framesPerRow, framesPerColumn, resource->m_KeyFrameWidth, resource->m_KeyFrameHeight);
		gameObject = shared_ptr<Sprite>(sprite);			
	} else
		gameObject = shared_ptr<GameObjectBase>(new GameObject(x, y));	

	gameObject->SetGraphicsResource(resource);
	gameObject->m_ColourKeyEnabled = colourKeyEnabled;
	gameObject->m_Visible = visible;		

	if(gameObject->m_ColourKeyEnabled)
		gameObject->SetColourKey(red,green,blue);
	if(resource->m_bIsAnimated)
		std::dynamic_pointer_cast<Sprite>(gameObject)->play();	
	return gameObject;
}
