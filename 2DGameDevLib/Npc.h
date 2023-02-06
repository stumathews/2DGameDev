#pragma once
#include "Hotspot.h"
#include "IGameObjectMoveStrategy.h"
#include "objects/AnimatedSprite.h"
#include "objects/DrawableGameObject.h"

class RoomInfo;

class Npc final : public gamelib::DrawableGameObject
{
public:
	Npc(const std::string& name, const std::string& type, gamelib::Coordinate<int> position,
	    bool visible, int inRoomNumber);
	void SetMoveStrategy(const std::shared_ptr<IGameObjectMoveStrategy>& inMoveStrategy);
	void SetSprite(const std::shared_ptr<gamelib::AnimatedSprite>& inSprite);
	std::shared_ptr<gamelib::AnimatedSprite> Sprite;
	int Width{};
	int Height{};
	void Draw(SDL_Renderer* renderer) override;
	gamelib::GameObjectType GetGameObjectType() override { return gamelib::GameObjectType::GameDefined;}
	void Update(float deltaMs) override;
	std::vector<std::shared_ptr<gamelib::Event>> HandleEvent(std::shared_ptr<gamelib::Event> event, unsigned long deltaMs) override;
	std::string GetSubscriberName() override { return "Npc";}
	std::string GetName() override {return "Npc1";}
	std::shared_ptr<RoomInfo> CurrentRoom;
	std::shared_ptr<gamelib::Hotspot> Hotspot;
private:	
	std::shared_ptr<IGameObjectMoveStrategy> moveStrategy;	
};

