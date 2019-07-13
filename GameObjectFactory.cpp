#include "GameObjectFactory.h"
#include "ResourceManager.h"
#include "Ball.h"
#include "Sprite.h"
using namespace tinyxml2;


shared_ptr<ActorBase> GameObjectFactory::BuildGameObject(tinyxml2::XMLElement * sceneObjectXml)
{
	unsigned int r = 0;
	unsigned int g = 0;
	unsigned int b = 0;
	unsigned int xPos, yPos = 0;
	bool visible, colourKeyEnabled = false;
	shared_ptr<GraphicsResource> resource;

	for(const XMLAttribute* sceneObjAtt = sceneObjectXml->FirstAttribute(); sceneObjAtt; sceneObjAtt = sceneObjAtt->Next()) {
		std::string attName = sceneObjAtt->Name();
		std::string attVal = sceneObjAtt->Value();		
		
		if(attName == "resourceId") {
			auto meta = ResourceManager::GetInstance().GetResourceByUuid(atoi(attVal.c_str()));
			if(meta->m_type != "graphic") { 
				throw new exception(("Cannot load non graphic resource: " + meta->m_name + " type=" + meta->m_type).c_str()); 
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

	
	auto gameObject = shared_ptr<ActorBase>();
	auto sprite = shared_ptr<Sprite>();
	if(resource->m_bIsAnimated) {

		sprite = shared_ptr<Sprite>(new Sprite(xPos, yPos, 100, 11, 3, 3));		
		gameObject = shared_ptr<ActorBase>(sprite);
		
	} else {
		gameObject = shared_ptr<ActorBase>(new GameObject(xPos, yPos));
	}


	
	gameObject->SetGraphicsResource(resource);
	gameObject->m_ColourKeyEnabled = colourKeyEnabled;
	gameObject->m_Visible = visible;
		

	if(gameObject->m_ColourKeyEnabled)
		gameObject->SetColourKey(r,g,b);

	if(resource->m_bIsAnimated)
	{
		
		sprite->play();
	}
	
	return gameObject;
}
