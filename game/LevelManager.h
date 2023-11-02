#pragma once

#include <memory>
#include "gameWorld.h"
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


/// <summary>
/// Does Level specific stuff
/// </summary>
class LevelManager : public gamelib::EventSubscriber
{
public:

    static LevelManager* Get();
    ~LevelManager() override;
    
    bool Initialize();
    [[nodiscard]] bool ChangeLevel(int levelNum) const; // Raises change level event
    static bool GetBoolSetting(const std::string& section, const std::string& settingName);
    void GetKeyboardInput(const unsigned long deltaMs) const;
    void OnFetchedPickup() const;
    void OnStartNetworkLevel(const std::shared_ptr<gamelib::Event>& evt);
    void OnNetworkPlayerJoined(const std::shared_ptr<gamelib::Event>& evt) const;
    void RemoveAllGameObjects();
    void OnLevelChanged(const std::shared_ptr<gamelib::Event>& evt) const;
    static void PlayLevelMusic(const std::string& levelMusicAssetName);
    static void InitializeHudItem(const std::shared_ptr<gamelib::StaticSprite>& hudItem);
    void InitializePlayer(const std::shared_ptr<Player>& inPlayer, const std::shared_ptr<gamelib::SpriteAsset>&
                          spriteAsset) const;
    void InitializeAutoPickups(const std::vector<std::shared_ptr<gamelib::Pickup>>& inPickups);
    void InitializeRooms(const std::vector<std::shared_ptr<Room>>& rooms);
    static std::string GetSetting(const std::string& section, const std::string& settingName);
    std::string GetSubscriberName() override { return "level_manager"; }
    void CreateAutoLevel(); // Raises level creation events
    void CreatePlayerStats();
    void CreateLevel(const std::string& levelFilePath); // Raises level creation events
    void CreateDrawableFrameRate();
    void CreateHud(const std::vector<std::shared_ptr<Room>>& rooms, const std::shared_ptr<Player>& inPlayer);
    static std::shared_ptr<gamelib::Asset> GetAsset(const std::string& name);
    static int GetIntSetting(const std::string& section, const std::string& settingName);
    std::shared_ptr<Level> GetLevel();
    static Mix_Chunk* GetSoundEffect(const std::string& name);

    void OnEnemyCollision(const std::shared_ptr<gamelib::Event>& evt);
    static void OnPlayerDied();
    void OnPickupCollision(const std::shared_ptr<gamelib::Event>& evt) const;
    gamelib::ListOfEvents HandleEvent(std::shared_ptr<gamelib::Event> evt, unsigned long inDeltaMs) override;
    std::shared_ptr<InputManager> GetInputManager();

protected:
    static LevelManager* instance;
    
private:
    void AddGameObjectToScene(const std::shared_ptr<gamelib::GameObject>& gameObject);
    void OnGameWon();
    void CreatePlayer(const std::vector<std::shared_ptr<Room>>& rooms, int resourceId);
    static std::shared_ptr<Room> GetRandomRoom(const std::vector<std::shared_ptr<Room>>& rooms);
    void CreateAutoPickups(const std::vector<std::shared_ptr<Room>>& rooms);
    void AddScreenWidgets(const std::vector<std::shared_ptr<Room>>& rooms);
    static size_t GetRandomIndex(const int min, const int max) { return rand() % (max - min + 1) + min; }     // NOLINT(concurrency-mt-unsafe)

    bool verbose = false;
    bool disableCharacters = false;
    unsigned int currentLevel = 1;
    gamelib::ProcessManager processManager;
    gamelib::EventManager* eventManager = nullptr;
    gamelib::EventFactory* eventFactory = nullptr;
    std::shared_ptr<InputManager> inputManager;
    std::shared_ptr<gamelib::StaticSprite> hudItem;
    std::shared_ptr<Level> level = nullptr;
    std::shared_ptr<gamelib::DrawableFrameRate> drawableFrameRate;
    std::shared_ptr<gamelib::DrawableText> playerPoints;
    std::shared_ptr<gamelib::DrawableText> playerHealth;
    std::shared_ptr<GameCommands> gameCommands;    
    std::shared_ptr<Player> player;
    std::shared_ptr<Enemy> enemy1;
    std::shared_ptr<Enemy> enemy2;
    std::vector<std::shared_ptr<gamelib::Pickup>> pickups;
    bool initialized {};
};


