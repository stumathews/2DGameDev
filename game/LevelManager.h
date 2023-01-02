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
#include "DrawableFrameRate.h"
#include <processes/ProcessManager.h>
#include <objects/StaticSprite.h>
#include "Pickup.h"

typedef std::vector<std::weak_ptr<gamelib::GameObject>> ListOfGameObjects;

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
    bool ChangeLevel(int levelNumber) const;
    static bool GetBoolSetting(const std::string& section, const std::string& settingName);
    void GetKeyboardInput() const;
    void OnFetchedPickup() const;
    void OnStartNetworkLevel(const std::shared_ptr<gamelib::Event>& evt);
    void OnNetworkPlayerJoined(const std::shared_ptr<gamelib::Event>& evt) const;
    void RemoveAllGameObjects();
    void OnLevelChanged(const std::shared_ptr<gamelib::Event>& evt) const;
    static void PlayLevelMusic(const std::string& levelMusicAssetName);
    static void InitializeHudItem(const std::shared_ptr<gamelib::StaticSprite>& hudItem);
    void InitializePlayer(const std::shared_ptr<Player>& inPlayer, const std::shared_ptr<gamelib::SpriteAsset>&
                          spriteAsset) const;
    void InitializePickups(const std::vector<std::shared_ptr<gamelib::Pickup>>& inPickups);
    void InitializeRooms(const std::vector<std::shared_ptr<Room>>& rooms);
    static std::string GetSetting(const std::string& section, const std::string& settingName);
    std::string GetSubscriberName() override { return "level_manager"; }
    void CreateAutoLevel();
    void CreateLevel(const std::string& filename);
    void CreateDrawableFrameRate();
    void CreateHUD(const std::vector<std::shared_ptr<Room>>& inRooms, const std::shared_ptr<Player>& inPlayer);
    static std::shared_ptr<gamelib::Asset> GetAsset(const std::string& name);
    static int GetIntSetting(const std::string& section, const std::string& settingName);
    std::shared_ptr<Level> GetLevel();
    static Mix_Chunk* GetSoundEffect(const std::string& name);

    gamelib::ListOfEvents HandleEvent(std::shared_ptr<gamelib::Event> evt, unsigned long inDeltaMs) override;


protected:
    static LevelManager* Instance;
    
private:
    void AddGameObjectToScene(const std::shared_ptr<gamelib::GameObject>& gameObject);
    void OnGameWon();
    void CreatePlayer(const std::vector<std::shared_ptr<Room>>& rooms, const std::shared_ptr<gamelib::SpriteAsset>&
                      playerSpriteAsset);
    void CreateAutoPickups(const std::vector<std::shared_ptr<Room>>& rooms, const int w, const int h);
    static size_t GetRandomIndex(const int min, const int max) { return rand() % (max - min + 1) + min; }   

    bool _verbose;
    int numLevelPickups = 0;
    unsigned int maxNumLevels = 5;
    unsigned long deltaMs;
    unsigned int currentLevel = 1;
    gamelib::ProcessManager processManager;
    gamelib::EventManager* _eventManager;
    gamelib::EventFactory* _eventFactory;
    std::shared_ptr<gamelib::StaticSprite> _hudItem;
    std::shared_ptr<Level> level = nullptr;
    std::shared_ptr<DrawableFrameRate> drawableFrameRate;
    std::shared_ptr<GameCommands> _gameCommands;
    std::shared_ptr<Player> player;
    std::vector<std::shared_ptr<gamelib::Pickup>> pickups;
};
