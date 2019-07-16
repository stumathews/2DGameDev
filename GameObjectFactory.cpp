#include "GameObjectFactory.h"
#include "ResourceManager.h"
#include "Ball.h"
#include "Sprite.h"
#include  "TypeAliases.h"
using namespace tinyxml2;


shared_ptr<GameObjectBase> GameObjectFactory::BuildGameObject(tinyxml2::XMLElement * sceneObjectXml)
{
	uint r = 0, g = 0, b = 0;
	
	uint xPos = 0, yPos = 0;
	bool visible = false, colourKeyEnabled = false;
	shared_ptr<GraphicsResource> resource;

	for(const XMLAttribute* sceneObjAtt = sceneObjectXml->FirstAttribute(); sceneObjAtt; sceneObjAtt = sceneObjAtt->Next()) {
		std::string attName = sceneObjAtt->Name();
		std::string attVal = sceneObjAtt->Value();		
		
		if(attName == "resourceId") {
			auto meta = ResourceManager::GetInstance().GetResourceByUuid(atoi(attVal.c_str()));
			if(meta->m_type != "graphic") { 
				throw new exception(("Cannot load non graphic resource: " + meta->m_name + " type=" + meta->m_type).c_str()); 
			}
			if(meta == NULL) {
				throw new exception(("Could not load resource meta data for resource id:" + attVal).c_str());
			}
			resource = std::dynamic_pointer_cast<GraphicsResource>(meta);
		}
		if(attName == "posx") {
			xPos = atoi(attVal.c_str());
			continue;
		}
		if(attName == "visible") {
			if(attVal == "true") {
				visible = true;
			} else {
				visible = false;
			}
		}
		if(attName=="posy") {
			yPos = atoi(attVal.c_str());
			continue;
		}
		if(attName == "colourKey") {
			if(attVal == "true") {
				colourKeyEnabled = true;
			} else {
				colourKeyEnabled = false;
			}
			continue;
		}
		if(attName == "r") {
			r = atof(attVal.c_str());
			continue;
		}
		if(attName == "g") {
			g = atof(attVal.c_str());
			continue;
		}
		if(attName == "b") {
			b = atof(attVal.c_str());
			continue;
		}		
	}
		
	auto gameObject = shared_ptr<GameObjectBase>(NULL);
	
	if(resource->m_bIsAnimated)
	{
		auto framesPerRow = 3, framesPerColumn = 3;
		auto sprite = new Sprite(xPos, yPos, 100, resource->m_NumKeyFrames, framesPerRow, framesPerColumn, resource->m_KeyFrameWidth, resource->m_KeyFrameHeight);
		gameObject = shared_ptr<Sprite>(sprite);			
	} else
		gameObject = shared_ptr<GameObjectBase>(new GameObject(xPos, yPos));	

	gameObject->SetGraphicsResource(resource);
	gameObject->m_ColourKeyEnabled = colourKeyEnabled;
	gameObject->m_Visible = visible;		

	if(gameObject->m_ColourKeyEnabled)
		gameObject->SetColourKey(r,g,b);
	if(resource->m_bIsAnimated)
		std::dynamic_pointer_cast<Sprite>(gameObject)->play();	
	return gameObject;
}
