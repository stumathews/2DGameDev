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

typedef std::vector<std::shared_ptr<gamelib::GameObject>> ListOfGameObjects;

class gamelib::GameWorldData;

class LevelManager : public gamelib::EventSubscriber
{
public:

    /// <summary>
    /// Construct a Level Manager
    /// </summary>
    LevelManager() = default;

    /// <summary>
    /// Initialize the level
    /// </summary>
    /// <returns></returns>
    bool Initialize();
    
    /// <summary>
    /// Get Controller input
    /// </summary>
    void GetKeyboardInput();

    /// <summary>
    /// Handle Level events
    /// </summary>
    std::vector<std::shared_ptr<gamelib::Event>> HandleEvent(std::shared_ptr<gamelib::Event> evt) override;

    void ProcessGameObjectEvent(std::shared_ptr<gamelib::Event>& evt);

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

    /// <summary>
    /// Starts the level 
    /// </summary>
    bool ChangeLevel(int levelNumber);
    
private:

    /// <summary>
    /// Create the Player
    /// </summary>
    std::shared_ptr<gamelib::GameObject> CreatePlayer(std::vector<std::shared_ptr<Room>> rooms, const int w, const int h) const;

    /// <summary>
    /// Create the Pickups
    /// </summary>
    ListOfGameObjects CreatePickups(const std::vector<std::shared_ptr<Room>>& rooms, const int w, const int h);
    
    /// <summary>
    /// Game commands
    /// </summary>
    std::shared_ptr<GameCommands> _gameCommands;

    /// <summary>
    /// Remove game object
    /// </summary>
    void RemoveGameObject(gamelib::GameObject& gameObject);
    
    /// <summary>
    /// Get Random Index between [min and max] 
    /// </summary>
    static size_t GetRandomIndex(const int min, const int max);	

    /// <summary>
    /// Set basic game world defaults
    /// </summary>
    void InitGameWorldData() const;
};
