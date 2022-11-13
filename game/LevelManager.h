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

    // Get Singelton access to LevelManager
    static LevelManager* Get();

    // Called on destruction of LevelManager (end of program)
    ~LevelManager();

    /// <summary>
    /// Initialize the level
    /// </summary>
    /// <returns></returns>
    bool Initialize();
    
    /// <summary>
    /// Get Controller input, and transaltes input to game commands
    /// </summary>
    void GetKeyboardInput();

    /// <summary>
    /// Handle Level events
    /// </summary>
    gamelib::ListOfEvents HandleEvent(std::shared_ptr<gamelib::Event> evt) override;

    void OnStartNetworkLevel(std::shared_ptr<gamelib::Event> evt);

    void OnNetworkPlayerJoined(std::shared_ptr<gamelib::Event> evt);

    /// <summary>
    /// Generates a new level
    /// </summary>
    void GenerateNewLevel();

    /// <summary>
    /// Takes game world objects and schedules them to be created
    /// </summary>
    void RemoveAllGameObjects();

    /// <summary>
    /// What happens when the level changed
    /// </summary>
    /// <param name="evt"></param>
    void OnLevelChanged(std::shared_ptr<gamelib::Event>& evt);

    /// <summary>
    /// Play level music
    /// </summary>
    /// <param name="levelMusicAssetName"></param>
    void PlayLevelMusic(std::string levelMusicAssetName);

    /// <summary>
    /// Name for event subsystem
    /// </summary>
    std::string GetSubscriberName() override;
    
    /// <summary>
    /// Create the Level's game objects
    /// </summary>
    ListOfGameObjects CreateAutoLevel();

    void InitializeHudItem(std::shared_ptr<StaticSprite> hudItem);

    void InitializePlayer(std::shared_ptr<Player> player, std::shared_ptr<gamelib::SpriteAsset> spriteAsset);

    // Subscribe pickups to events, add to scene graph etc
    void InitializePickups(const ListOfGameObjects& pickups, ListOfGameObjects& gameObjectsPtr);

    // Subscribe rooms to events, add to scene graph etc
    void InitializeRooms(std::vector<std::shared_ptr<Room>>& rooms, ListOfGameObjects& gameObjectsPtr);


    /// <summary>
    /// Create a level from a level file
    /// </summary>
    /// <param name="filename">path to level file</param>
    /// <returns>ListOfGameObjects</returns>
    ListOfGameObjects CreateLevel(std::string filename);

    /// <summary>
    /// Starts the level 
    /// </summary>
    bool ChangeLevel(int levelNumber);

    // Reduces the pickup count and returns the current pickup acount as a result
    int ReducePickupCount();

    // Increases the pickup count and returns the current pickup acount as a result
    int IncreasePickupCount();

    std::shared_ptr<Level> GetLevel();

    void RegisterGameObject(std::shared_ptr<gamelib::GameObject> obj);

protected:

    // Singleton Instance of LevelManager
    static LevelManager* Instance;
    
private:

    /// <summary>
    /// Create the Player
    /// </summary>
    std::shared_ptr<Player> CreatePlayer(std::vector<std::shared_ptr<Room>> rooms, const int w, const int h) const;

    /// <summary>
    /// Create the Pickups
    /// </summary>
    ListOfGameObjects CreatePickups(const std::vector<std::shared_ptr<Room>>& rooms, const int w, const int h);
        
    /// <summary>
    /// Game commands - all game commands that we handle in this game.
    /// </summary>
    std::shared_ptr<GameCommands> _gameCommands;
    unsigned int currentLevel = 1;
    gamelib::EventManager* eventManager;
    gamelib::EventFactory* eventFactory;

    /// <summary>
    /// Remove game object
    /// </summary>
    void RemoveGameObject(gamelib::GameObject& gameObject);
    
    /// <summary>
    /// Get Random Index between [min and max] 
    /// </summary>
    static size_t GetRandomIndex(const int min, const int max);	

    /// <summary>
    /// Be verbose or not in logging
    /// </summary>
    bool verbose;

    int numLevelPickups = 0;

    /// <summary>
    /// Hud item at the bottom right of the screen
    /// </summary>
    std::shared_ptr<StaticSprite> hudItem;

    /// <summary>
    /// Current level
    /// </summary>
    std::shared_ptr<Level> level = nullptr;

    std::shared_ptr<DrawableFrameRate> drawableFrameRate;

    void AddGameObjectToScene(std::shared_ptr<gamelib::GameObject> object);

    unsigned long deltaMs;
    gamelib::ProcessManager processManager;
};
