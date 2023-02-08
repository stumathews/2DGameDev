#pragma once
#include "Hotspot.h"
#include "IGameObjectMoveStrategy.h"
#include "objects/AnimatedSprite.h"
#include "objects/DrawableGameObject.h"
#include <memory>

#include "ai/FSM.h"

class RoomInfo;

class Npc final : public gamelib::DrawableGameObject, public std::enable_shared_from_this<Npc>
{
public:
	Npc(const std::string& name, const std::string& type, gamelib::Coordinate<int> position, bool visible,
	    const std::shared_ptr<Room>& startRoom, std::shared_ptr<gamelib::AnimatedSprite> sprite,
	    std::shared_ptr<IGameObjectMoveStrategy> moveStrategy = nullptr);

	void Initialize();
		
	void Draw(SDL_Renderer* renderer) override;
	gamelib::GameObjectType GetGameObjectType() override { return gamelib::GameObjectType::GameDefined;}
	void Update(unsigned long deltaMs) override;
	std::vector<std::shared_ptr<gamelib::Event>> HandleEvent(std::shared_ptr<gamelib::Event> event, unsigned long deltaMs) override;
	void SetNpcDirection(gamelib::Direction direction);
	std::string GetSubscriberName() override { return "Npc";}
	std::string GetName() override { return "Npc1";}

	// Fields
	std::shared_ptr<gamelib::AnimatedSprite> Sprite;
	gamelib::ABCDRectangle Dimensions{};
	std::shared_ptr<RoomInfo> CurrentRoom;
	std::shared_ptr<gamelib::Hotspot> Hotspot;

private:
	gamelib::FSM stateMachine;
	std::shared_ptr<IGameObjectMoveStrategy> moveStrategy;	
	gamelib::Direction currentMovingDirection;
	gamelib::Direction currentFacingDirection;
	gamelib::FSMState upState;	
	gamelib::FSMState downState;	
	gamelib::FSMState leftState;	
	gamelib::FSMState rightState;
	gamelib::FSMTransition onUpDirection;
	gamelib::FSMTransition onDownDirection;
	gamelib::FSMTransition onLeftDirection;
	gamelib::FSMTransition onRightDirection;
};

