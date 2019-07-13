#pragma once
#include <memory>
#include "tinyxml2.h"
#include "Actor.h"

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
	std::shared_ptr<ActorBase> BuildGameObject(tinyxml2::XMLElement * sceneObjectXml);
private:
	static GameObjectFactory& m_Instance;
	GameObjectFactory(){};
};

