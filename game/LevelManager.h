#pragma once

#include <memory>
#include "audio/AudioManager.h"
#include "graphic/SDLGraphicsManager.h"
#include "resource/ResourceManager.h"
#include "GameCommands.h"
#include "Room.h"
#include <events/EventFactory.h>
#include "graphic/DrawableFrameRate.h"
#include <processes/ProcessManager.h>
#include <character/StaticSprite.h>
#include "Enemy.h"
#include "InputManager.h"
#include "Pickup.h"
#include "graphic/DrawableText.h"

typedef std::vector<std::weak_ptr<gamelib::GameObject>> ListOfGameObjects;

class Level;

class LevelManager final : public gamelib::EventSubscriber
{
public:
    LevelManager& operator=(const LevelManager& other) = delete;
    LevelManager& operator=(const LevelManager&& other) = delete;
    LevelManager(const LevelManager& other) = delete;
    LevelManager(const LevelManager&& other) = delete;
    LevelManager() = default;
    ~LevelManager() override;

    bool Initialize();
    [[nodiscard]] bool ChangeLevel(int levelNum) const; // Raises change level event
    gamelib::ListOfEvents HandleEvent(std::shared_ptr<gamelib::Event> evt, unsigned long inDeltaMs) override;
    static bool GetBoolSetting(const std::string& section, const std::string& settingName);
    static int GetIntSetting(const std::string& section, const std::string& settingName);
    static LevelManager* Get();
    static Mix_Chunk* GetSoundEffect(const std::string& name);
    static std::shared_ptr<gamelib::Asset> GetAsset(const std::string& name);
    static std::string GetSetting(const std::string& section, const std::string& settingName);
    static void InitializeHudItem(const std::shared_ptr<gamelib::StaticSprite>& hudItem);
    static void OnPlayerDied();
    static void PlayLevelMusic(const std::string& levelMusicAssetName);
    std::shared_ptr<InputManager> GetInputManager();
    std::shared_ptr<Level> GetLevel();
    std::string GetSubscriberName() override;
    void CreateAutoLevel(); // Raises level creation events
    void CreateDrawableFrameRate();
    void CreateHud(const std::vector<std::shared_ptr<Room>>& rooms, const std::shared_ptr<Player>& inPlayer);
    void CreateLevel(const std::string& levelFilePath); // Raises level creation events
    void CreatePlayerStats();
    void GetKeyboardInput(const unsigned long deltaMs) const;
    void InitializeAutoPickups(const std::vector<std::shared_ptr<gamelib::Pickup>>& inPickups);
    void InitializePlayer(const std::shared_ptr<Player>& inPlayer, const std::shared_ptr<gamelib::SpriteAsset>& spriteAsset) const;
    void InitializeRooms(const std::vector<std::shared_ptr<Room>>& rooms);
    void OnEnemyCollision(const std::shared_ptr<gamelib::Event>& evt);
    void OnFetchedPickup() const;
    void OnLevelChanged(const std::shared_ptr<gamelib::Event>& evt) const;
    void OnNetworkPlayerJoined(const std::shared_ptr<gamelib::Event>& evt) const;
    void OnPickupCollision(const std::shared_ptr<gamelib::Event>& evt) const;
    void OnStartNetworkLevel(const std::shared_ptr<gamelib::Event>& evt);
    void RemoveAllGameObjects();

protected:
    static LevelManager* instance;
    
private:

    bool disableCharacters = false;
    bool initialized {};
    bool verbose = false;
    gamelib::EventFactory* eventFactory = nullptr;
    gamelib::EventManager* eventManager = nullptr;
    gamelib::ProcessManager processManager;
    static size_t GetRandomIndex(const int min, const int max) { return rand() % (max - min + 1) + min; }     // NOLINT(concurrency-mt-unsafe)
    static std::shared_ptr<Room> GetRandomRoom(const std::vector<std::shared_ptr<Room>>& rooms);
    std::shared_ptr<Enemy> enemy1;
    std::shared_ptr<Enemy> enemy2;
    std::shared_ptr<GameCommands> gameCommands;    
    std::shared_ptr<gamelib::DrawableFrameRate> drawableFrameRate;
    std::shared_ptr<gamelib::DrawableText> playerHealth;
    std::shared_ptr<gamelib::DrawableText> playerPoints;
    std::shared_ptr<gamelib::StaticSprite> hudItem;
    std::shared_ptr<InputManager> inputManager;
    std::shared_ptr<Level> level = nullptr;
    std::shared_ptr<Player> player;
    std::vector<std::shared_ptr<gamelib::Pickup>> pickups;
    unsigned int currentLevel = 1;
    void AddGameObjectToScene(const std::shared_ptr<gamelib::GameObject>& gameObject);
    void AddScreenWidgets(const std::vector<std::shared_ptr<Room>>& rooms);
    void CreateAutoPickups(const std::vector<std::shared_ptr<Room>>& rooms);
    void CreatePlayer(const std::vector<std::shared_ptr<Room>>& rooms, int resourceId);
    void OnGameWon();
};


