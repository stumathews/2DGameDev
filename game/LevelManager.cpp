#include "LevelManager.h"
#include "common/Common.h"
#include "Pickup.h"
#include "common/constants.h"
#include "events/AddGameObjectToCurrentSceneEvent.h"
#include "LevelGenerator.h"
#include "objects/game_world_component.h"
#include <events/GameObjectEvent.h>
#include "Player.h"
#include <memory>
#include <events/SceneChangedEvent.h>
#include <SpriteAsset.h>
#include <objects/GameObjectFactory.h>
#include "SnapToRoomStrategy.h"
#include "EdgeTowardsRoomStrategy.h"
#include "GameObjectEventFactory.h"
#include <GameData.h>
#include <sstream>

using namespace gamelib;
using namespace std;

bool LevelManager::Initialize()
{
	InitGameWorldData();

	_gameCommands = shared_ptr<GameCommands>(new GameCommands());

	EventManager::Get()->SubscribeToEvent(EventType::GenerateNewLevel, this);
	EventManager::Get()->SubscribeToEvent(EventType::InvalidMove, this);
	EventManager::Get()->SubscribeToEvent(EventType::FetchedPickup, this);
	EventManager::Get()->SubscribeToEvent(EventType::GameObject, this);
	EventManager::Get()->SubscribeToEvent(EventType::LevelChangedEventType, this);

	verbose = SettingsManager::Get()->GetBool("global", "verbose");
		
	return true;
}

gamelib::ListOfEvents LevelManager::HandleEvent(std::shared_ptr<Event> event)
{
	gamelib::ListOfEvents newEvents;
	std::shared_ptr<GameObjectEvent> gameObjectEvent = nullptr;
	std::stringstream output; 
	
	switch(event->type)
	{
	case EventType::InvalidMove:
		_gameCommands->InvalidMove();
		break;
	case EventType::FetchedPickup:
		_gameCommands->FetchedPickup();
		break;		
	case EventType::GenerateNewLevel:
		GenerateNewLevel();
		break;
	case EventType::LevelChangedEventType:
		OnLevelChanged(event);
		break;
	case EventType::GameObject:
		gameObjectEvent = dynamic_pointer_cast<GameObjectEvent>(event);
		switch(gameObjectEvent->context)
		{
			case GameObjectEventContext::Remove:
				RemoveGameObject(*gameObjectEvent->gameObject);
				break;
			default:				
				output << "Unknown Game Object Event:" << ToString(gameObjectEvent->context);
				gamelib::Logger::Get()->LogThis(output.str());
		}
		break;
	default:
		output << "Unknown EventType:" << ToString(event->type);
		gamelib::Logger::Get()->LogThis(output.str());

	}

	return newEvents;
}

void LevelManager::GenerateNewLevel()
{
	RemoveAllGameObjects();
	CreateAutoLevel();
}

void LevelManager::RemoveAllGameObjects()
{
	std::for_each(std::begin(SceneManager::Get()->GetGameWorld().GetGameObjects()), std::end(SceneManager::Get()->GetGameWorld().GetGameObjects()), [this](std::shared_ptr<gamelib::GameObject> gameObject) 
	{
		EventManager::Get()->RaiseEvent(GameObjectEventFactory::MakeRemoveObjectEvent(&(*gameObject)), this);
	});

	//Reclassify game objects

	GameData::Get()->SetGameObjects(&SceneManager::Get()->GetGameWorld().GetGameObjects());
}

void LevelManager::OnLevelChanged(std::shared_ptr<gamelib::Event>& evt)
{
	const auto levelChangedEvent = dynamic_pointer_cast<SceneChangedEvent>(evt);
	auto level = levelChangedEvent->scene_id;

	switch(level)
	{
		case 1:
			PlayLevelMusic("LevelMusic1");
			break;
		case 2:
			PlayLevelMusic("LevelMusic2");
			break;
		case 3:
			PlayLevelMusic("LevelMusic3");
			break;
		case 4:
			PlayLevelMusic("LevelMusic4");
			break;
		default:
			std::stringstream message;
			message << "Unexpected level " << level;
			Logger::Get()->LogThis(message.str());
	}
}

void LevelManager::PlayLevelMusic(std::string levelMusicAssetName)
{
	auto asset = ResourceManager::Get()->GetAssetInfo(levelMusicAssetName);
	if (asset->isLoadedInMemory)
	{
		auto audioAsset = gamelib::AudioManager::ToAudioAsset(asset);
		if (asset && asset->isLoadedInMemory)
		{
			AudioManager::Get()->Play(audioAsset->AsMusic());
		}
	}
}

std::string LevelManager::GetSubscriberName() 
{ 
	return "level_manager";
};

void LevelManager::RemoveGameObject(gamelib::GameObject& gameObject)
{
	auto& gameObjects = SceneManager::Get()->GetGameWorld().GetGameObjects();
	
	// Look for gameObject
	auto findResult = std::find_if(begin(gameObjects), end(gameObjects), [&](weak_ptr<gamelib::GameObject> target)
	{ 
		if(auto underlyingGameObject = target.lock()) // Test to see if the game object reference still exists 
		{
			// Found object we're looking for?
			return underlyingGameObject->Id == gameObject.Id;
		}
		return false;
	});
	
	auto found = findResult != end(gameObjects);
			
	if(found)
	{
		EventManager::Get()->Unsubscribe((*findResult)->Id);
		gameObjects.erase(findResult);
	}

	GameData::Get()->SetGameObjects(&gameObjects);
}

void LevelManager::InitGameWorldData() const
{	
	SceneManager::Get()->GetGameWorld().IsGameDone = false;
	SceneManager::Get()->GetGameWorld().IsNetworkGame = false;
	SceneManager::Get()->GetGameWorld().CanDraw = true;
}

void LevelManager::GetKeyboardInput()
{
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent))
	{
		if (sdlEvent.type == SDL_KEYDOWN)
		{
			switch (sdlEvent.key.keysym.sym)
			{
			case SDLK_SPACE:				
				_gameCommands->Fire(verbose);
				break;
			case SDLK_w:
			case SDLK_UP:				
				_gameCommands->MoveUp(verbose);
				break;
			case SDLK_s:
			case SDLK_DOWN:				
				_gameCommands->MoveDown(verbose);
				break;
			case SDLK_a:
			case SDLK_LEFT:				
				_gameCommands->MoveLeft(verbose);
				break;
			case SDLK_d:
			case SDLK_RIGHT:				
				_gameCommands->MoveRight(verbose);
				break;
			case SDLK_q:
			case SDLK_ESCAPE:
				_gameCommands->Quit(verbose);
				break;
			case SDLK_j:	
				GenerateNewLevel();
				_gameCommands->ChangeLevel(verbose, 1);
				break;
			case SDLK_k:
				GenerateNewLevel();
				_gameCommands->ChangeLevel(verbose, 2);
				break;
			case SDLK_l:
				GenerateNewLevel();
				_gameCommands->ChangeLevel(verbose, 3);
				break;
			case SDLK_x:
				GenerateNewLevel();
				_gameCommands->ChangeLevel(verbose, 4);
				break;
			case SDLK_1:
				_gameCommands->PlaySoundEffect(gamelib::AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo("high.wav"))->AsSoundEffect());				
				break;
			case SDLK_2:
				_gameCommands->PlaySoundEffect(gamelib::AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo("medium.wav"))->AsSoundEffect());
				break;
			case SDLK_3:
				_gameCommands->PlaySoundEffect(gamelib::AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo("low.wav"))->AsSoundEffect());
				break;
			case SDLK_4:
				_gameCommands->PlaySoundEffect(gamelib::AudioManager::ToAudioAsset(ResourceManager::Get()->GetAssetInfo("scratch.wav"))->AsSoundEffect());
				break;
			case SDLK_9:
				_gameCommands->ToggleMusic(verbose);
				break;
			case SDLK_r:				
				_gameCommands->ReloadSettings(verbose);
				break;
			case SDLK_g:				
				_gameCommands->GenerateNewLevel(verbose);
				break;
			default:
				std::cout << "Unknown control key" << std::endl;
				LogMessage("Unknown control key", verbose);
				break;
			}
		}
		
		if (sdlEvent.type == SDL_QUIT)
		{
			_gameCommands->Quit(verbose);
			return;
		}
	}
}

size_t LevelManager::GetRandomIndex(const int min, const int max) { return rand() % (max - min + 1) + min; }

shared_ptr<gamelib::GameObject> LevelManager::CreatePlayer(const vector<shared_ptr<Room>> rooms, const int playerWidth, const int playerHeight) const
{	
	const auto minNumRooms = 0;
	const auto playerRoomIndex = GetRandomIndex(minNumRooms, rooms.size());
	const auto playerRoom = rooms[playerRoomIndex];
	const auto positionInRoom = playerRoom->GetCenter(playerWidth, playerHeight);	
	const auto player =  shared_ptr<Player>(new Player(positionInRoom.GetX(), positionInRoom.GetY(), playerWidth, playerHeight));	
	auto moveStrategy = SettingsManager::Get()->GetString("player", "moveStrategy");

	std::string spriteAssetName;

	GetPlayerAssetName(moveStrategy, player, spriteAssetName);

	player->SetRoom(playerRoomIndex);
	player->SetTag(constants::playerTag);
	player->LoadSettings();
	player->RaiseEvent(std::make_shared<AddGameObjectToCurrentSceneEvent>(player, 100));
	player->CenterPlayerInRoom(playerRoom);

	auto spriteAsset = dynamic_pointer_cast<SpriteAsset>(ResourceManager::Get()->GetAssetInfo(spriteAssetName));
	auto sprite = AnimatedSprite::Create(player->x, player->y, spriteAsset);
		
	player->SetSprite(sprite);

	// We keep track of the player globally
	SceneManager::Get()->GetGameWorld().player = player;

	return player;
}

void LevelManager::GetPlayerAssetName(std::string& moveStrategy, const std::shared_ptr<Player>& player, std::string& spriteAssetName) const
{
	if (moveStrategy == "snap")
	{
		player->SetMoveStrategy(shared_ptr<SnapToRoomStrategy>(new SnapToRoomStrategy(player)));
		spriteAssetName = "snap_player";
	}
	else if (moveStrategy == "edge")
	{
		player->SetMoveStrategy(shared_ptr<EdgeTowardsRoomStrategy>(new EdgeTowardsRoomStrategy(player, 2)));
		spriteAssetName = "edge_player";
	}
	else
	{
		THROW(12, "Unknown Move strategy", "Create Player");
	}
}

ListOfGameObjects LevelManager::CreatePickups(const vector<shared_ptr<Room>>& rooms, const int pickupWidth, const int pickupHeight)
{
	ListOfGameObjects pickups;
	const auto numPickups = SettingsManager::Get()->GetInt("global", "numPickups");
	auto part = numPickups / 3;

	for(auto i = 0; i < numPickups; i++)
	{
		const auto rand_index = GetRandomIndex(0, rooms.size()-1);
		const auto random_room = rooms[rand_index];	
		const auto positionInRoom = random_room->GetCenter(pickupWidth, pickupHeight);

		auto pickup = std::shared_ptr<Pickup>(new Pickup(positionInRoom.GetX(), positionInRoom.GetY(), pickupWidth, pickupHeight, true));

		// Place 3 sets evently of each type of pickup	
		if(i < 1 * part) 
		{
			pickup->stringProperties["assetName"] = SettingsManager::Get()->GetString("pickup1", "assetName");		
		}
		else if(i >= 1 * part && i < 2 * part) 
		{ 
			pickup->stringProperties["assetName"] = SettingsManager::Get()->GetString("pickup2", "assetName"); 
		}
		else if(i >= 2 * part)
		{
			pickup->stringProperties["assetName"] = SettingsManager::Get()->GetString("pickup3", "assetName");
		}

		pickup->Initialize();
		pickups.push_back(pickup);
	}
	return pickups;
}

ListOfGameObjects LevelManager::CreateAutoLevel()
{
	const auto rows = SettingsManager::Get()->GetInt("grid", "rows");
	const auto cols = SettingsManager::Get()->GetInt("grid", "cols");
	const auto screenWidth = SettingsManager::Get()->GetInt("global", "screen_width");
	const auto screenHeight = SettingsManager::Get()->GetInt("global", "screen_height");
	const auto removeRandomSides = SettingsManager::Get()->GetBool("grid", "removeSidesRandomly");
	const auto rowWidth = screenWidth / cols; 
	const auto rowHeight = screenHeight / rows;

	ResourceManager::Get()->IndexResourceFile();
	
	ListOfGameObjects& gameObjectsPtr = SceneManager::Get()->GetGameWorld().GetGameObjects();
	
	// Generate the level's rooms
	auto levelGenerator = LevelGenerator(screenWidth, screenHeight, rows, cols, removeRandomSides);
	auto rooms = levelGenerator.Generate();

	for (const auto& room : rooms)
	{		
		room->LoadSettings();
		room->SubscribeToEvent(EventType::PlayerMovedEventType);
		room->RaiseEvent(std::make_shared<AddGameObjectToCurrentSceneEvent>(std::dynamic_pointer_cast<Room>(room)));
		room->SubscribeToEvent(EventType::SettingsReloaded);

		gameObjectsPtr.push_back(dynamic_pointer_cast<gamelib::GameObject>(room));
	}
			
	// Create the player
	const auto playerWidth = rowWidth/2;
	const auto playerHeight = rowHeight/2;
	const auto player = CreatePlayer(rooms, playerWidth, playerHeight);

	// Create the pickups
	const auto pickupWidth = rowWidth/2;
	const auto pickupHeight = rowHeight/2;
	const auto pickups = CreatePickups(rooms, pickupWidth, pickupHeight);

	// Setup the pickups
	for(const auto &pickup : pickups)
	{
		auto gameObject = std::dynamic_pointer_cast<Pickup>(pickup);
		auto addEvent = new AddGameObjectToCurrentSceneEvent(gameObject);
		
		pickup->SubscribeToEvent(EventType::PlayerMovedEventType);
		pickup->SubscribeToEvent(gamelib::EventType::DoLogicUpdateEventType);		
		pickup->RaiseEvent(std::shared_ptr<AddGameObjectToCurrentSceneEvent>(addEvent));

		gameObjectsPtr.push_back(pickup);		
	}
	
	// Add player to game world
	gameObjectsPtr.push_back(player);	

	GameData::Get()->SetGameObjects(&gameObjectsPtr);
		
	return gameObjectsPtr;
}

bool LevelManager::ChangeLevel(int level)
{
	bool isSuccess = false;
	try
	{
		_gameCommands->ChangeLevel(false, level);
		isSuccess = true;
	}
	catch (...)
	{
		LogMessage("Could not start level for unknown reasons. level=" + std::to_string(level));
	}
	return isSuccess;
}
