#include "GameObjectFactory.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Sprite.h"
#include  "TypeAliases.h"
#include "GameObject.h"
#include "Square.h"
using namespace tinyxml2;

/*
Create a Game Object Type from an object's xml definition.
*/
shared_ptr<game_object> GameObjectFactory::BuildGameObject(tinyxml2::XMLElement * sceneObjectXml)
{
	uint red = 0, green = 0, blue = 0;	
	uint m_xPos = 0, m_yPos = 0;
	bool visible = false, colourKeyEnabled = false;
	shared_ptr<GraphicsResource> resource;
	
	// The game object we are going to construct
	auto emptyGameObject = shared_ptr<game_object>(NULL);

	// Traverse the object definition
	for(const XMLAttribute* sceneObjAtt = sceneObjectXml->FirstAttribute(); sceneObjAtt; sceneObjAtt = sceneObjAtt->Next()) 
	{
		std::string detailName = sceneObjAtt->Name();
		std::string detailValue = sceneObjAtt->Value();		
		
		// Discover object's information:

		if(detailName == "resourceId") 
		{
			// Get the associated resource details referenced by this object
			auto meta = resource_manager::get().get_resource_by_uuid(atoi(detailValue.c_str()));
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
			m_xPos = atoi(detailValue.c_str());
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
			m_yPos = atoi(detailValue.c_str());
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
	return InitGameObject(emptyGameObject, m_xPos, m_yPos, resource, colourKeyEnabled, visible, red, green, blue);		
}

std::shared_ptr<game_object>& GameObjectFactory::InitGameObject(std::shared_ptr<game_object>& gameObject, uint m_xPos, uint m_yPos, std::shared_ptr<GraphicsResource>& resource, bool colourKeyEnabled, bool visible, const uint& red, const uint& green, const uint& blue)
{	
	if(resource == NULL)
	{ 
		auto width = 50;
		gameObject = shared_ptr<game_object>(new square(m_xPos, m_yPos, width));
		gameObject->is_visible = true;
		return gameObject;		
	}

	if( (red < 0 || red > 255)  || (blue < 0 || blue > 255) || (green < 0 || green > 255) )
	{
		throw new exception("Invalid colour values when constructing game object");
	}

	if( m_xPos < 0 || m_yPos < 0)
	{
		throw new exception("Position values are invalid when constructing a game object");
	}


	// Special animated game object?	
	if(resource->m_bIsAnimated)
	{
		const auto frames_per_row = 3;
		const auto frames_per_column = 3;
		const auto the_sprite = new sprite(m_xPos, m_yPos, 100, resource->m_NumKeyFrames, frames_per_row, frames_per_column, resource->m_KeyFrameWidth, resource->m_KeyFrameHeight);
		gameObject = shared_ptr<sprite>(the_sprite);
		
	} 
	else
	{
		// Normal 2d Sprite (no animation)
		gameObject = shared_ptr<game_object>(new sprite(m_xPos, m_yPos));	
	}
	gameObject->set_graphic_resource(resource);
	gameObject->is_color_key_enabled = colourKeyEnabled;
	gameObject->is_visible = visible;

	if (gameObject->is_color_key_enabled)
		gameObject->set_color_key(red, green, blue);


	if (resource->m_bIsAnimated)
		std::dynamic_pointer_cast<sprite>(gameObject)->play();
	return gameObject;
}
