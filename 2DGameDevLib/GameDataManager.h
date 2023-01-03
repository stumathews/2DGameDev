#pragma once
#include <events\EventSubscriber.h>
#include "GameData.h"
#include <events/EventManager.h>
#include <events/EventFactory.h>

class GameDataManager final : public gamelib::EventSubscriber
{
public:
	static GameDataManager* Get() { if (Instance == nullptr) { Instance = new GameDataManager(); } return Instance; }
	~GameDataManager() override { Instance = nullptr; }

	std::vector<std::shared_ptr<gamelib::Event>> HandleEvent(std::shared_ptr<gamelib::Event> evt, unsigned long deltaMs) override;
	std::string GetSubscriberName() override { return "GameDataManager"; }
	static GameData* GameData() { return GameData::Get(); }
	void Initialize();
protected:
	static GameDataManager* Instance;
private:
	GameDataManager();
	void AddToGameData(const std::shared_ptr<gamelib::Event>& evt) const;
	void RemoveFromGameData(const std::shared_ptr<gamelib::Event>& evt);
	void RemoveGameObject(const std::shared_ptr<gamelib::GameObject>& gameObject) const;

	gamelib::EventManager* eventManager;
	gamelib::EventFactory* eventFactory;
};

