#pragma once
#include <events\EventSubscriber.h>
#include "GameData.h"
#include <events/EventManager.h>
#include <events/EventFactory.h>
class GameDataManager : public gamelib::EventSubscriber
{
public:
	static GameDataManager* Get() { if (Instance == nullptr) { Instance = new GameDataManager(); } return Instance; }
	~GameDataManager() { Instance = nullptr; }
	virtual std::vector<std::shared_ptr<gamelib::Event>> HandleEvent(std::shared_ptr<gamelib::Event> evt, unsigned long deltaMs) override;
	virtual std::string GetSubscriberName() override { return "GameDataManager"; };
	GameData* GameData() { return GameData::Get(); }
	void Initialize();
protected:
	static GameDataManager* Instance;
private:
	GameDataManager();
	void AddToGameData(std::shared_ptr<gamelib::Event> evt);
	void RemoveFromGameData(std::shared_ptr<gamelib::Event> evt);
	void RemoveGameObject(std::shared_ptr<gamelib::GameObject> gameObject);

	gamelib::EventManager* _eventManager;
	gamelib::EventFactory* _eventFactory;
};

