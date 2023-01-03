#include "LevelManager.h"
#include "common/Common.h"
#include "common/constants.h"
#include "objects/game_world_component.h"
#include "Player.h"
#include <memory>
#include <events/SceneChangedEvent.h>
#include <objects/GameObjectFactory.h>
#include "PlayerMoveStrategy.h"
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
#include "events/AddGameObjectToCurrentSceneEvent.h"

using namespace gamelib;
using namespace std;

bool LevelManager::Initialize()
{
	GameData::Get()->IsGameDone = false;
	GameData::Get()->IsNetworkGame = false;
	GameData::Get()->CanDraw = true;

	_eventManager = EventManager::Get();
	_eventFactory = EventFactory::Get();
	_gameCommands = std::make_shared<GameCommands>();

	_eventManager->SubscribeToEvent(EventType::GenerateNewLevel, this);
	_eventManager->SubscribeToEvent(EventType::InvalidMove, this);
	_eventManager->SubscribeToEvent(EventType::FetchedPickup, this);
	_eventManager->SubscribeToEvent(EventType::GameObject, this);
	_eventManager->SubscribeToEvent(EventType::LevelChangedEventType, this);
	_eventManager->SubscribeToEvent(EventType::NetworkPlayerJoined, this);
	_eventManager->SubscribeToEvent(EventType::StartNetworkLevel, this);
	_eventManager->SubscribeToEvent(EventType::UpdateProcesses, this);
	_eventManager->SubscribeToEvent(EventType::GameWon, this);

	_verbose =  GetBoolSetting("global", "verbose");

	GameData::Get()->IsNetworkGame = GetBoolSetting("global", "isNetworkGame");
			
	return true;
}

ListOfEvents LevelManager::HandleEvent(const std::shared_ptr<Event> evt, const unsigned long inDeltaMs)
{
	switch(evt->Type)  // NOLINT(clang-diagnostic-switch-enum)
	{
		case EventType::LevelChangedEventType: { OnLevelChanged(evt); } break;
		case EventType::UpdateProcesses: { processManager.UpdateProcesses(inDeltaMs); } break;
		case EventType::InvalidMove: { _gameCommands->InvalidMove(); } break;
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
	_gameCommands->FetchedPickup();
	_hudItem->AdvanceFrame();	
}

void LevelManager::OnStartNetworkLevel(const std::shared_ptr<Event>& evt)
{
	// read the start level event and create the level
	// ReSharper disable once CppExpressionWithoutSideEffects
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
			_eventManager->RaiseEvent(GameObjectEventFactory::MakeRemoveObjectEvent(gameObject.lock()), this);
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
	if (asset && asset->isLoadedInMemory) { AudioManager::Get()->Play(AudioManager::ToAudioAsset(asset)->AsMusic()); }	
}

void LevelManager::OnGameWon()
{
	const auto a = std::static_pointer_cast<Process>(
		std::make_shared<Action>([&]() { _gameCommands->ToggleMusic(_verbose); }));
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
		_gameCommands->LoadNewLevel(static_cast<int>(++currentLevel));
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

	if (keyState[SDL_SCANCODE_UP] || keyState[SDL_SCANCODE_W]) { _gameCommands->MoveUp(_verbose); }
	if (keyState[SDL_SCANCODE_DOWN] || keyState[SDL_SCANCODE_S]) { _gameCommands->MoveDown(_verbose); }
	if (keyState[SDL_SCANCODE_LEFT] || keyState[SDL_SCANCODE_A]) { _gameCommands->MoveLeft(_verbose); }
	if (keyState[SDL_SCANCODE_RIGHT] || keyState[SDL_SCANCODE_D]) { _gameCommands->MoveRight(_verbose); }
		
	while (SDL_PollEvent(&sdlEvent))
	{		
		if (sdlEvent.type == SDL_KEYDOWN)
		{
			switch (sdlEvent.key.keysym.sym)
			{						
				case SDLK_q: 
				case SDLK_ESCAPE: _gameCommands->Quit(_verbose); break;
				case SDLK_r: _gameCommands->ReloadSettings(_verbose); break;
				case SDLK_p: _gameCommands->PingGameServer(); break;
				case SDLK_n: _gameCommands->StartNetworkLevel(); break;
				case SDLK_SPACE: _gameCommands->Fire(_verbose); break;
			default: /* Do nothing */;
			}
		}
		
		if (sdlEvent.type == SDL_QUIT)  { _gameCommands->Quit(_verbose); return; }
	}
}

void LevelManager::CreatePlayer(const vector<shared_ptr<Room>> &rooms, const shared_ptr<SpriteAsset>& playerSpriteAsset) 
{
	const auto playerNickName = GameData::Get()->IsNetworkGame
								?  GetSetting("networking", "nickname")
								: "Player1";

	player = std::make_shared<Player>("player1", "playerType", rooms[GetRandomIndex(0, static_cast<int>(rooms.size()) - 1)],
	                                  playerSpriteAsset->Dimensions.GetWidth(),
	                                  playerSpriteAsset->Dimensions.GetHeight(), playerNickName);

	InitializePlayer(player, playerSpriteAsset);
	AddGameObjectToScene(player);
}

void LevelManager::CreateAutoPickups(const vector<shared_ptr<Room>>& rooms, const int pickupWidth, const int pickupHeight)
{
	const auto numPickups = GetIntSetting("global", "numPickups");
	const auto part = numPickups / 3;

	for(auto i = 0; i < numPickups; i++)
	{
		const auto rand_index = GetRandomIndex(0, static_cast<int>(rooms.size())-1);
		const auto& random_room = rooms[rand_index];
		const auto positionInRoom = random_room->GetCenter(pickupWidth, pickupHeight);
		const auto pickupName = string("RoomPickup") + std::to_string(random_room->GetRoomNumber());

		auto pickup = std::make_shared<Pickup>(pickupName, "Pickup", positionInRoom.GetX(),
		                                       positionInRoom.GetY(), pickupWidth, pickupHeight, true,
		                                       rand_index);
		
		// Place 3 sets evenly of each type of pickup	
		if(i < 1 * part)  {	pickup->stringProperties["assetName"] = GetSetting("pickup1", "assetName"); }
		else if(i >= 1 * part && i < 2 * part) { pickup->stringProperties["assetName"] = GetSetting("pickup2", "assetName"); }
		else if(i >= 2 * part) { pickup->stringProperties["assetName"] = GetSetting("pickup3", "assetName"); }

		pickup->Initialize();
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

	const auto rowWidth = GetIntSetting("global", "screen_width") / level->NumCols;
	const auto rowHeight = GetIntSetting("global", "screen_height") / level->NumRows;
	const auto pickupWidth = rowWidth / 2;
	const auto pickupHeight = rowHeight / 2;

	const auto& rooms = level->Rooms;
	InitializeRooms(rooms);	

	CreatePlayer(rooms, dynamic_pointer_cast<SpriteAsset>(GetAsset("edge_player")));	
	CreateHUD(rooms, player);

	if (level->IsAutoPopulatePickups())
	{
		CreateAutoPickups(rooms, pickupWidth, pickupHeight);
	}

	CreateDrawableFrameRate();
}

void LevelManager::CreateDrawableFrameRate()
{
	drawableFrameRate = std::make_shared<DrawableFrameRate>(&level->Rooms[level->Rooms.size() - 2]->Bounds);
	AddGameObjectToScene(drawableFrameRate);
}

void LevelManager::CreateHUD(const std::vector<std::shared_ptr<Room>>& inRooms, const std::shared_ptr<Player>& inPlayer)
{
	// ReSharper disable once StringLiteralTypo
	_hudItem = GameObjectFactory::Get().BuildStaticSprite("","", GetAsset("hudspritesheet"), inRooms[inRooms.size() - 1]->GetCenter(inPlayer->GetWidth(), inPlayer->GetHeight()));
	InitializeHudItem(_hudItem);
	AddGameObjectToScene(_hudItem);
}



void LevelManager::InitializeHudItem(const std::shared_ptr<StaticSprite>& hudItem)
{
	hudItem->LoadSettings();
	hudItem->SubscribeToEvent(EventType::PlayerMovedEventType);
}

void LevelManager::AddGameObjectToScene(const std::shared_ptr<GameObject>& object) { _eventManager->RaiseEvent(std::dynamic_pointer_cast<Event>(_eventFactory->CreateAddToSceneEvent(object)), this); }

void LevelManager::CreateAutoLevel()
{
	ResourceManager::Get()->IndexResourceFile(); 
	RemoveAllGameObjects();

	// Register change level event
	// ReSharper disable once CppExpressionWithoutSideEffects
	Get()->ChangeLevel(1);

	level = std::make_shared<Level>();
	level->Load();

	InitializeRooms(level->Rooms);
	CreatePlayer(level->Rooms, dynamic_pointer_cast<SpriteAsset>(GetAsset("edge_player")));
	CreateAutoPickups(level->Rooms, 32, 32);
	CreateHUD(level->Rooms, player);
}

void LevelManager::InitializePlayer(const std::shared_ptr<Player>& inPlayer, const std::shared_ptr<SpriteAsset>&
                                    spriteAsset) const
{
	inPlayer->SetMoveStrategy(std::make_shared<PlayerMoveStrategy>(inPlayer, 2));
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
		_gameCommands->RaiseChangedLevel(false, static_cast<short>(levelNumber));
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
LevelManager* LevelManager::Get() { if (Instance == nullptr) { Instance = new LevelManager(); } return Instance; }
LevelManager::~LevelManager() { Instance = nullptr; }
LevelManager* LevelManager::Instance = nullptr;