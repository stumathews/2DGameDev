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
	LevelManager();

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

	void GenerateNewLevel();

	void OnLevelChanged(std::shared_ptr<gamelib::Event>& evt);

	void PlayLevelMusic(std::string levelMusicAssetName);

	/// <summary>
	/// Name for event subsystem
	/// </summary>
	std::string GetSubscriberName() override;
	
	/// <summary>
	/// Create the Level's game objects
	/// </summary>
	ListOfGameObjects CreateLevel();	

	/// <summary>
	/// Starts the level 
	/// </summary>
	bool PrepareLevel(int levelNumber);
	
private:

	/// <summary>
	/// Create the Player
	/// </summary>
	std::shared_ptr<gamelib::GameObject> CreatePlayer(std::vector<std::shared_ptr<Room>> rooms, const int w, const int h) const;

	/// <summary>
	/// Create the Pickups
	/// </summary>
	ListOfGameObjects CreatePickups(const std::vector<std::shared_ptr<Room>>& rooms, const int w, const int h);
	
	// Game commands
	std::shared_ptr<GameCommands> _gameCommands;

    /// <summary>
    /// Remove game object
    /// </summary>
    void RemoveGameObject(gamelib::GameObject& gameObject);
	
	/// <summary>
	/// Get Random Index between [min and max] 
	/// </summary>
	static size_t GetRandomIndex(const int min, const int max);	

	// set basic game world defaults
	void InitGameWorldData() const;
};
