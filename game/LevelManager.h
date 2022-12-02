#pragma once

#include <memory>
#include "util/SettingsManager.h"
#include "gameWorld.h"
#include "audio/AudioManager.h"
#include "graphic/SDLGraphicsManager.h"
#include "resource/ResourceManager.h"
#include "scene/SceneManager.h"
#include "GameCommands.h"
#include "Room.h"
#include <events/EventFactory.h>
#include <objects/StaticSprite.h>
#include "DrawableFrameRate.h"
#include <processes/ProcessManager.h>
#
typedef std::vector<std::shared_ptr<gamelib::GameObject>> ListOfGameObjects;

class gamelib::GameWorldData;
class gamelib::EventManager;
class Level;


/// <summary>
/// Does Level specific stuff
/// </summary>
class LevelManager : public gamelib::EventSubscriber
{
public:

    static LevelManager* Get();
    ~LevelManager();
    
    bool Initialize();
    bool ChangeLevel(int levelNumber);
    bool GetBoolSetting(std::string section, std::string settingName) const;
    void GetKeyboardInput();
    void OnGameObjectEventReceived(std::shared_ptr<gamelib::Event>& event);
    void OnFetchedPickup();
    void OnStartNetworkLevel(std::shared_ptr<gamelib::Event> evt);
    void OnNetworkPlayerJoined(std::shared_ptr<gamelib::Event> evt);
    void GenerateNewLevel();
    void RemoveAllGameObjects();
    void OnLevelChanged(std::shared_ptr<gamelib::Event>& evt);
    void PlayLevelMusic(std::string levelMusicAssetName);
    void InitializeHudItem(std::shared_ptr<StaticSprite> _hudItem);
    void InitializePlayer(std::shared_ptr<Player> player, std::shared_ptr<gamelib::SpriteAsset> spriteAsset);
    void InitializePickups(const ListOfGameObjects& pickups, ListOfGameObjects& gameObjectsPtr);
    void InitializeRooms(std::vector<std::shared_ptr<Room>>& rooms, ListOfGameObjects& gameObjectsPtr);
    void RegisterGameObject(std::shared_ptr<gamelib::GameObject> obj);
    std::string GetSetting(std::string section, std::string settingName) const;
    std::string GetSubscriberName() override { return "level_manager"; }
    ListOfGameObjects CreateAutoLevel();    
    ListOfGameObjects CreateLevel(std::string filename);
    const std::shared_ptr<gamelib::Asset> GetAsset(std::string name) const;    
    int ReducePickupCount();
    int IncreasePickupCount();
    int GetIntSetting(std::string section, std::string settingName) const;
    std::shared_ptr<Level> GetLevel();
    Mix_Chunk* GetSoundEffect(std::string name);

    gamelib::ListOfEvents HandleEvent(std::shared_ptr<gamelib::Event> evt, unsigned long deltaMs) override;


protected:

    // Singleton Instance of LevelManager
    static LevelManager* Instance;
    
private:
    void AddGameObjectToScene(const std::shared_ptr<gamelib::GameObject> gameObject);
    void RemoveGameObject(gamelib::GameObject& gameObject);
    std::shared_ptr<Player> CreatePlayer(const std::vector<std::shared_ptr<Room>> rooms, std::shared_ptr<gamelib::SpriteAsset> playerSpriteAsset) const;
    ListOfGameObjects CreatePickups(const std::vector<std::shared_ptr<Room>>& rooms, const int w, const int h);
    static size_t GetRandomIndex(const int min, const int max) { return rand() % (max - min + 1) + min; }   

    bool _verbose;
    int numLevelPickups = 0;
    unsigned long deltaMs;
    unsigned int currentLevel = 1;
    gamelib::ProcessManager processManager;
    gamelib::EventManager* _eventManager;
    gamelib::EventFactory* _eventFactory;
    std::shared_ptr<StaticSprite> _hudItem;
    std::shared_ptr<Level> level = nullptr;
    std::shared_ptr<DrawableFrameRate> drawableFrameRate;
    std::shared_ptr<GameCommands> _gameCommands;
};
