#include "LevelManager.h"
#include "common/Common.h"
#include "common/constants.h"
#include "objects/game_world_component.h"
#include "Player.h"
#include <memory>
#include <events/SceneChangedEvent.h>
#include <objects/GameObjectFactory.h>
#include "GameObjectEventFactory.h"
#include <GameData.h>
#include <events/NetworkPlayerJoinedEvent.h>
#include <Level.h>
#include "util/RectDebugging.h"
#include "windows.h"
#include <processes/DelayProcess.h>
#include <processes/Action.h>
#include <functional>
#include <events/Event.h>

// ReSharper disable once CppUnusedIncludeDirective
#include <random>

#include "CharacterBuilder.h"
#include "GameDataManager.h"
#include "GameObjectMoveStrategy.h"

using namespace gamelib;
using namespace std;

bool LevelManager::Initialize()
{
	GameData::Get()->IsGameDone = false;
	GameData::Get()->IsNetworkGame = false;
	GameData::Get()->CanDraw = true;

	eventManager = EventManager::Get();
	eventFactory = EventFactory::Get();
	gameCommands = std::make_shared<GameCommands>();

	eventManager->SubscribeToEvent(EventType::GenerateNewLevel, this);
	eventManager->SubscribeToEvent(EventType::InvalidMove, this);
	eventManager->SubscribeToEvent(EventType::FetchedPickup, this);
	eventManager->SubscribeToEvent(EventType::GameObject, this);
	eventManager->SubscribeToEvent(EventType::LevelChangedEventType, this);
	eventManager->SubscribeToEvent(EventType::NetworkPlayerJoined, this);
	eventManager->SubscribeToEvent(EventType::StartNetworkLevel, this);
	eventManager->SubscribeToEvent(EventType::UpdateProcesses, this);
	eventManager->SubscribeToEvent(EventType::GameWon, this);

	verbose =  GetBoolSetting("global", "verbose");

	GameData::Get()->IsNetworkGame = GetBoolSetting("global", "isNetworkGame");
			
	return true;
}

ListOfEvents LevelManager::HandleEvent(const std::shared_ptr<Event> evt, const unsigned long inDeltaMs)
{
	switch(evt->Type)  // NOLINT(clang-diagnostic-switch-enum)
	{
		case EventType::LevelChangedEventType: { OnLevelChanged(evt); } break;
		case EventType::UpdateProcesses: { processManager.UpdateProcesses(inDeltaMs); } break;
		case EventType::InvalidMove: { gameCommands->InvalidMove(); } break;
		case EventType::NetworkPlayerJoined: { 	OnNetworkPlayerJoined(evt); }	break;
		case EventType::StartNetworkLevel: { OnStartNetworkLevel(evt); }	break;
		case EventType::FetchedPickup: { OnFetchedPickup(); } break;	
		case EventType::GameWon: { OnGameWon(); } break;
	default: /* Do Nothing */;
	}

	return {};
}

void LevelManager::OnFetchedPickup() const
{
	gameCommands->FetchedPickup();
	hudItem->AdvanceFrame();	
}

void LevelManager::OnStartNetworkLevel(const std::shared_ptr<Event>& evt)
{
	// read the start level event and create the level
	// ReSharper disable once CppNoDiscardExpression
	ChangeLevel(1);

	// Network games all start on level 1 for now
	CreateLevel(GetSetting("global", "level1FileName"));
}

string LevelManager::GetSetting(const std::string& section, const std::string& settingName) { return SettingsManager::Get()->GetString(section, settingName); }
int LevelManager::GetIntSetting(const std::string& section, const std::string& settingName) { return SettingsManager::Get()->GetInt(section, settingName); }
bool LevelManager::GetBoolSetting(const std::string& section, const std::string& settingName) { return SettingsManager::Get()->GetBool(section, settingName); }

void LevelManager::RemoveAllGameObjects()
{
	std::for_each(std::begin(GameData::Get()->GameObjects), std::end(GameData::Get()->GameObjects), [this](const std::weak_ptr<
		              GameObject>
	              & gameObject)
	{
		if (!gameObject.expired())
		{
			eventManager->RaiseEvent(GameObjectEventFactory::MakeRemoveObjectEvent(gameObject.lock()), this);
		}
	});

	pickups.clear();
	player = nullptr;
}

void LevelManager::OnLevelChanged(const std::shared_ptr<Event>& evt) const
{
	switch(dynamic_pointer_cast<SceneChangedEvent>(evt)->SceneId)
	{
		case 1: PlayLevelMusic("LevelMusic1"); break;
		case 2: PlayLevelMusic("LevelMusic2"); break;
		case 3: PlayLevelMusic("LevelMusic3"); break;
		case 4: PlayLevelMusic("LevelMusic4"); break;
		default: PlayLevelMusic("AutoLevelMusic"); break;
	}
}

void LevelManager::PlayLevelMusic(const std::string& levelMusicAssetName)
{
	// ReSharper disable once CppTooWideScopeInitStatement
	const auto asset = ResourceManager::Get()->GetAssetInfo(levelMusicAssetName);
	if (asset && asset->IsLoadedInMemory) { AudioManager::Get()->Play(AudioManager::ToAudioAsset(asset)->AsMusic()); }	
}

void LevelManager::OnGameWon()
{
	const auto a = std::static_pointer_cast<Process>(
		std::make_shared<Action>([&]() { gameCommands->ToggleMusic(verbose); }));
	const auto b = std::static_pointer_cast<Process>(std::make_shared<Action>([&]()
	{
		AudioManager::Get()->Play(  // NOLINT(readability-static-accessed-through-instance)
			AudioManager::ToAudioAsset(
				ResourceManager::Get()->GetAssetInfo(SettingsManager::Get()->GetString("audio", "win_music")))->
			AsSoundEffect());
	}));
	
	const auto c = std::static_pointer_cast<Process>(std::make_shared<DelayProcess>(5000));

	const auto d = std::static_pointer_cast<Process>(std::make_shared<Action>([&]()
	{
		gameCommands->LoadNewLevel(static_cast<int>(++currentLevel));
	}));

	// Chain a set of subsequent processes
	a->AttachChild(b); 
		b->AttachChild(c); 
			c->AttachChild(d);

	processManager.AttachProcess(a);

	Logger::Get()->LogThis("All Pickups Collected Well Done!");	
}

void LevelManager::GetKeyboardInput() const
{
	SDL_Event sdlEvent;
	const auto keyState = SDL_GetKeyboardState(nullptr);

	if (keyState[SDL_SCANCODE_UP] || keyState[SDL_SCANCODE_W]) { gameCommands->MoveUp(verbose); }
	if (keyState[SDL_SCANCODE_DOWN] || keyState[SDL_SCANCODE_S]) { gameCommands->MoveDown(verbose); }
	if (keyState[SDL_SCANCODE_LEFT] || keyState[SDL_SCANCODE_A]) { gameCommands->MoveLeft(verbose); }
	if (keyState[SDL_SCANCODE_RIGHT] || keyState[SDL_SCANCODE_D]) { gameCommands->MoveRight(verbose); }
		
	while (SDL_PollEvent(&sdlEvent))
	{		
		if (sdlEvent.type == SDL_KEYDOWN)
		{
			switch (sdlEvent.key.keysym.sym)
			{						
				case SDLK_q: 
				case SDLK_ESCAPE: gameCommands->Quit(verbose); break;
				case SDLK_r: gameCommands->ReloadSettings(verbose); break;
				case SDLK_p: gameCommands->PingGameServer(); break;
				case SDLK_n: gameCommands->StartNetworkLevel(); break;
				case SDLK_SPACE: gameCommands->Fire(verbose); break;
			default: /* Do nothing */;
			}
		}
		
		if (sdlEvent.type == SDL_QUIT)  { gameCommands->Quit(verbose); return; }
	}
}

void LevelManager::CreatePlayer(const vector<shared_ptr<Room>> &rooms, const int resourceId) 
{
	const auto playerNickName = GameData::Get()->IsNetworkGame
	?  GetSetting("networking", "nickname")
	    : "Player1";

	player = CharacterBuilder::BuildPlayer("Player1", rooms[GetRandomIndex(0, static_cast<int>(rooms.size()) - 1)], resourceId, playerNickName);

	AddGameObjectToScene(player);
}

shared_ptr<Room> LevelManager::GetRandomRoom(const std::vector<std::shared_ptr<Room>>& rooms)
{
	return rooms[GetRandomIndex(0, static_cast<int>(rooms.size()) - 1)];
}

void LevelManager::CreateNpcs(const std::vector<std::shared_ptr<Room>>& rooms, const int resourceId)
{
	for(auto i = 0; i < 10; i++)
	{
		const auto enemy = CharacterBuilder::BuildEnemy("Enemy" + std::to_string(i), GetRandomRoom(rooms), resourceId, GetRandomDirection());
		enemy->Initialize();
		GameDataManager::Get()->GameData()->AddEnemy(enemy);
		AddGameObjectToScene(enemy);
	}
}

void LevelManager::CreateAutoPickups(const vector<shared_ptr<Room>>& rooms)
{
	const auto numPickups = GetIntSetting("global", "numPickups");
	const auto part = numPickups / 3;

	for(auto i = 0; i < numPickups; i++)
	{
		const auto& randomRoom = rooms[GetRandomIndex(0, static_cast<int>(rooms.size())-1)];
		const auto pickupName = string("RoomPickup") + std::to_string(randomRoom->GetRoomNumber());
		
		std::shared_ptr<SpriteAsset> spriteAssert;

		// Place 3 sets evenly of each type of pickup	
		if(i < 1 * part)
		{
			spriteAssert = dynamic_pointer_cast<SpriteAsset>(ResourceManager::Get()->GetAssetInfo(GetSetting("pickup1", "assetName")));
		}
		else if(i >= 1 * part && i < 2 * part)
		{
			spriteAssert = dynamic_pointer_cast<SpriteAsset>(ResourceManager::Get()->GetAssetInfo(GetSetting("pickup2", "assetName")));
		}
		else if(i >= 2 * part)
		{
			spriteAssert = dynamic_pointer_cast<SpriteAsset>(ResourceManager::Get()->GetAssetInfo(GetSetting("pickup3", "assetName")));
		}

		auto pickup = CharacterBuilder::BuildPickup(pickupName, randomRoom, spriteAssert->uid);
		
		pickups.push_back(pickup);
	}

	// Setup the pickups
	InitializePickups(pickups);
}

void LevelManager::CreateLevel(const string& filename)
{	
	RemoveAllGameObjects();

	// Load the level definition file
	level = std::make_shared<Level>(filename);	
	level->Load();

	const auto& rooms = level->Rooms;
	InitializeRooms(rooms);	

	CreatePlayer(rooms, GetAsset("edge_player")->uid);
	//CreateNpcs(rooms, GetAsset("snap_player")->uid);
	CreateHud(rooms, player);

	if (level->IsAutoPopulatePickups())
	{
		CreateAutoPickups(rooms);
	}

	CreateDrawableFrameRate();
}

void LevelManager::CreateDrawableFrameRate()
{
	drawableFrameRate = std::make_shared<DrawableFrameRate>(&level->Rooms[level->Rooms.size() - 2]->Bounds);
	AddGameObjectToScene(drawableFrameRate);
}

void LevelManager::CreateHud(const std::vector<std::shared_ptr<Room>>& rooms, const std::shared_ptr<Player>& inPlayer)
{
	// ReSharper disable once StringLiteralTypo
	hudItem = GameObjectFactory::Get().BuildStaticSprite("","", GetAsset("hudspritesheet"), rooms[rooms.size() - 1]->GetCenter(inPlayer->GetWidth(), inPlayer->GetHeight()));
	InitializeHudItem(hudItem);
	AddGameObjectToScene(hudItem);
}



void LevelManager::InitializeHudItem(const std::shared_ptr<StaticSprite>& hudItem)
{
	hudItem->LoadSettings();
	hudItem->SubscribeToEvent(EventType::PlayerMovedEventType);
}

void LevelManager::AddGameObjectToScene(const std::shared_ptr<GameObject>& gameObject) { eventManager->RaiseEvent(std::dynamic_pointer_cast<Event>(eventFactory->CreateAddToSceneEvent(gameObject)), this); }

void LevelManager::CreateAutoLevel()
{
	ResourceManager::Get()->IndexResourceFile(); 
	RemoveAllGameObjects();

	// Register change level event
	// ReSharper disable once CppNoDiscardExpression
	Get()->ChangeLevel(1);  // NOLINT(clang-diagnostic-unused-result)

	level = std::make_shared<Level>();
	level->Load();

	InitializeRooms(level->Rooms);
	CreatePlayer(level->Rooms, GetAsset("edge_player")->uid);
	CreateAutoPickups(level->Rooms);
	CreateHud(level->Rooms, player);
}



void LevelManager::InitializePlayer(const std::shared_ptr<Player>& inPlayer, const std::shared_ptr<SpriteAsset>&
                                    spriteAsset) const
{
	inPlayer->SetMoveStrategy(std::make_shared<GameObjectMoveStrategy>(inPlayer, inPlayer->CurrentRoom));
	inPlayer->SetTag(constants::PlayerTag);
	inPlayer->LoadSettings();
	inPlayer->SetSprite(AnimatedSprite::Create(inPlayer->Position, spriteAsset));

	// We keep a reference to track of the player globally
	GameData::Get()->player = inPlayer;
}

void LevelManager::InitializePickups(const std::vector<std::shared_ptr<Pickup>>& inPickups)
{
	for (const auto& pickup : inPickups)
	{
		pickup->LoadSettings();
		pickup->SubscribeToEvent(EventType::PlayerMovedEventType);

		AddGameObjectToScene(pickup);
	}
}

void LevelManager::InitializeRooms(const std::vector<std::shared_ptr<Room>>& rooms)
{

	for (const auto& room : rooms)
	{
		room->LoadSettings();
		room->SubscribeToEvent(EventType::PlayerMovedEventType);
		room->SubscribeToEvent(EventType::SettingsReloaded);

		AddGameObjectToScene(room);
	}
}

bool LevelManager::ChangeLevel(const int levelNumber) const
{
	bool isSuccess = false;
	try
	{
		gameCommands->RaiseChangedLevel(false, static_cast<short>(levelNumber));
		isSuccess = true;
	}
	catch (...) { LogMessage("Could not start level for unknown reasons. level=" + std::to_string(levelNumber)); 	}
	return isSuccess;
}

void LevelManager::OnNetworkPlayerJoined(const std::shared_ptr<Event>& evt) const
{
	// We know a player has joined the game server.
	const auto networkPlayerJoinedEvent = dynamic_pointer_cast<NetworkPlayerJoinedEvent>(evt);
	
	// Add the player to our level... find a suitable position for it
}

Mix_Chunk* LevelManager::GetSoundEffect(const std::string& name) { return AudioManager::ToAudioAsset(GetAsset(name))->AsSoundEffect(); }
std::shared_ptr<Asset> LevelManager::GetAsset(const std::string& name) { return ResourceManager::Get()->GetAssetInfo(name); }

shared_ptr<Level> LevelManager::GetLevel() { return level; }
LevelManager* LevelManager::Get() { if (instance == nullptr) { instance = new LevelManager(); } return instance; }
LevelManager::~LevelManager() { instance = nullptr; }
LevelManager* LevelManager::instance = nullptr;