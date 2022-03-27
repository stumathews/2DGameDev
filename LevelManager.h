#pragma once

#include <memory>
#include "util/SettingsManager.h"
#include "game/gameWorld.h"
#include "audio/AudioManager.h"
#include "graphic/SDLGraphicsManager.h"
#include "resource/ResourceManager.h"
#include "scene/SceneManager.h"
#include "GameCommands.h"
#include <scene/Room.h>

class LevelManager : public gamelib::EventSubscriber
{
public:

	/// <summary>
	/// Construct a Level Manager
	/// </summary>
	LevelManager(GameWorld& gameWorld, gamelib::SceneManager& scene_admin);

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

	/// <summary>
	/// Name for event subsystem
	/// </summary>
	std::string GetSubscriberName() override;
	
	/// <summary>
	/// Create the Level's game objects
	/// </summary>
	gamelib::ListOfGameObjects CreateLevel();	

	/// <summary>
	/// Starts the level 
	/// </summary>
	bool PrepareLevel(int levelNumber);
	
private:

	/// <summary>
	/// Create the Player
	/// </summary>
	std::shared_ptr<gamelib::GameObject> CreatePlayer(std::vector<std::shared_ptr<gamelib::Room>> rooms, const int w, const int h) const;

	/// <summary>
	/// Create the Pickups
	/// </summary>
	gamelib::ListOfGameObjects CreatePickups(const std::vector<std::shared_ptr<gamelib::Room>>& rooms, const int w, const int h);
	
	// Game world, including game objects
	GameWorld& gameWorld;

	gamelib::SceneManager& sceneManager;
	
	// Game commands
	std::shared_ptr<GameCommands> _gameCommands;

    /// <summary>
    /// Remove game object
    /// </summary>
    void RemoveGameObject(GameWorld& gameWorld, gamelib::GameObject& gameObject);
	
	/// <summary>
	/// Get Random Index between [min and max] 
	/// </summary>
	static size_t GetRandomIndex(const int min, const int max);	

	// set basic game world defaults
	void InitGameWorldData() const;
};
