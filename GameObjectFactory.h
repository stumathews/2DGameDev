#pragma once
#include <memory>
#include "tinyxml2.h"
#include "GameObjectBase.h"
#include "TypeAliases.h"
#include "GameObject.h"

class GameObjectFactory
{
public:
	 static GameObjectFactory& GetInstance()
        {
            static GameObjectFactory instance;			
            return instance;
        }
	 GameObjectFactory(GameObjectFactory const&)  = delete;
     void operator=(GameObjectFactory const&)  = delete;
	 ~GameObjectFactory(){};
	std::shared_ptr<GameObject> BuildGameObject(tinyxml2::XMLElement * sceneObjectXml);
	std::shared_ptr<GameObject>& InitGameObject(std::shared_ptr<GameObject>& gameObject, uint x, uint y, std::shared_ptr<GraphicsResource>& resource, bool colourKeyEnabled, bool visible, const uint& red, const uint& green, const uint& blue);
private:
	static GameObjectFactory& m_Instance;
	GameObjectFactory(){};
};

