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
#include "graphic/RectDebugging.h"
#include <processes/DelayProcess.h>
#include <processes/Action.h>
#include <functional>
#include <cppgamelib/events.h>

// ReSharper disable once CppUnusedIncludeDirective
#include <random>
#include <events/UpdateProcessesEvent.h>

#include "CharacterBuilder.h"
#include "EventNumber.h"
#include "GameObjectMoveStrategy.h"
#include "PlayerCollidedWithEnemyEvent.h"
#include "PlayerCollidedWithPickupEvent.h"
#include "events/StartNetworkLevelEvent.h"
#include "graphic/DrawableFrameRate.h"
#include "graphic/DrawableText.h"
#include "scene/SceneManager.h"

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

	eventManager->SubscribeToEvent(GenerateNewLevelEventId, this);
	eventManager->SubscribeToEvent(InvalidMoveEventId, this);
	eventManager->SubscribeToEvent(FetchedPickupEventId, this);
	eventManager->SubscribeToEvent(GameObjectTypeEventId, this);
	eventManager->SubscribeToEvent(LevelChangedEventTypeEventId, this);
	eventManager->SubscribeToEvent(NetworkPlayerJoinedEventId, this);
	eventManager->SubscribeToEvent(StartNetworkLevelEventId, this);
	eventManager->SubscribeToEvent(UpdateProcessesEventId, this);
	eventManager->SubscribeToEvent(GameWonEventId, this);
	eventManager->SubscribeToEvent(PlayerCollidedWithEnemyEventId, this);
	eventManager->SubscribeToEvent(PlayerDiedEventId, this);
	eventManager->SubscribeToEvent(PlayerCollidedWithPickupEventId, this);

	verbose =  GetBoolSetting("global", "verbose");

	GameData::Get()->IsNetworkGame = GetBoolSetting("global", "isNetworkGame");
			
	return true;
}

ListOfEvents LevelManager::HandleEvent(const std::shared_ptr<Event> evt, const unsigned long inDeltaMs)
{
	if(evt->Id.PrimaryId == LevelChangedEventTypeEventId.PrimaryId) { OnLevelChanged(evt); }
	if(evt->Id.PrimaryId == UpdateProcessesEventId.PrimaryId) { processManager.UpdateProcesses(inDeltaMs); }
	if(evt->Id.PrimaryId == InvalidMoveEventId.PrimaryId) { gameCommands->InvalidMove();}
	if(evt->Id.PrimaryId == NetworkPlayerJoinedEventId.PrimaryId) { OnNetworkPlayerJoined(evt);}
	if(evt->Id.PrimaryId == StartNetworkLevelEventId.PrimaryId) { OnStartNetworkLevel(evt); }
	if(evt->Id.PrimaryId == FetchedPickupEventId.PrimaryId) { OnFetchedPickup();}
	if(evt->Id.PrimaryId == GameWonEventId.PrimaryId) { OnGameWon();}
	if(evt->Id.PrimaryId == PlayerCollidedWithEnemyEventId.PrimaryId) { OnEnemyCollision(evt);}
	if(evt->Id.PrimaryId == PlayerDiedEventId.PrimaryId) { OnPlayerDied(); }
	if(evt->Id.PrimaryId == PlayerCollidedWithPickupEventId.PrimaryId) { OnPickupCollision(evt); }

	return {};
}

void LevelManager::OnEnemyCollision(const std::shared_ptr<Event>& evt) 
{
	gameCommands->FetchedPickup();
	const auto collisionEvent = std::dynamic_pointer_cast<PlayerCollidedWithEnemyEvent>(evt);
	const auto& enemyHitPoints = collisionEvent->Enemy->StringProperties["HitPoint"];
	collisionEvent->Player->IntProperties["Health"] -= strtol(enemyHitPoints.c_str(), nullptr, 0);
	auto msg = to_string(collisionEvent->Player->GetHealth());
	Logger::Get()->LogThis(msg);
	playerHealth->Text = msg;
	if(collisionEvent->Player->GetHealth() <= 0)
	{
		
		eventManager->RaiseEvent(std::dynamic_pointer_cast<Event>(eventFactory->CreateGenericEvent(PlayerDiedEventId)), this);
	}
}

void LevelManager::OnPickupCollision(const std::shared_ptr<Event>& evt) const
{
	const auto collisionEvent = std::dynamic_pointer_cast<PlayerCollidedWithPickupEvent>(evt);
	const auto& pickupValue = collisionEvent->Pickup->StringProperties["value"];
	collisionEvent->Player->IntProperties["Points"] += strtol(pickupValue.c_str(), nullptr, 0);
	const auto msg = to_string(collisionEvent->Player->GetPoints());
	playerPoints->Text = msg;
	Logger::Get()->LogThis(msg);
}


void LevelManager::OnFetchedPickup() const
{
	gameCommands->FetchedPickup();
	hudItem->AdvanceFrame();	
}


void LevelManager::OnPlayerDied()
{
	Logger::Get()->LogThis("Player DIED!");
}

void LevelManager::OnStartNetworkLevel(const std::shared_ptr<Event>& evt)
{
	// read the start level event and create the level
	// ReSharper disable once CppNoDiscardExpression
	auto _ = ChangeLevel(1);  // NOLINT(clang-diagnostic-unused-result)

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
		case 5: PlayLevelMusic("LevelMusic5"); break;
		default: PlayLevelMusic("AutoLevelMusic"); break;
	}
}

void LevelManager::PlayLevelMusic(const std::string& levelMusicAssetName)
{
	// ReSharper disable once CppTooWideScopeInitStatement
	const auto asset = ResourceManager::Get()->GetAssetInfo(levelMusicAssetName);
	if (asset && asset->IsLoadedInMemory) { AudioManager::Get()->Play(asset); }	
}

void LevelManager::OnGameWon()
{
	const auto a = std::static_pointer_cast<Process>(
		std::make_shared<Action>([&]() { gameCommands->ToggleMusic(verbose); }));
	const auto b = std::static_pointer_cast<Process>(std::make_shared<Action>([&]()
	{
		AudioManager::Get()->Play(ResourceManager::Get()->GetAssetInfo(SettingsManager::String("audio", "win_music")));
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

	const auto msg = "All Pickups Collected Well Done!";
	Logger::Get()->LogThis(msg);	
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
				case SDLK_0: gameCommands->ToggleMusic(false); break;
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

		auto pickup = CharacterBuilder::BuildPickup(pickupName, randomRoom, spriteAssert->Uid);
		
		pickups.push_back(pickup);
	}

	// Setup the pickups
	InitializeAutoPickups(pickups);
}

void LevelManager::CreateLevel(const string& levelFilePath)
{	
	RemoveAllGameObjects();

	// Load the level definition file
	level = std::make_shared<Level>(levelFilePath);	
	level->Load();

	// Setup rooms in the level
	const auto& rooms = level->Rooms;
	InitializeRooms(rooms);	

	// Add player to room in level
	CreatePlayer(rooms, GetAsset("edge_player")->Uid);

	// Add Hud
	CreateHud(rooms, player);

	if (level->IsAutoPopulatePickups())
	{
		CreateAutoPickups(rooms);
	}

	// Add FrameRate to scene
	CreateDrawableFrameRate();

	// Add Player stats to scene
	CreatePlayerStats();
}

void LevelManager::CreateDrawableFrameRate()
{
	const int lastColumn = level->NumCols;

	constexpr auto firstRow = 1;
	auto widgetArea = &level->GetRoom(firstRow,lastColumn)->Bounds;

	// Add frame rate
	drawableFrameRate = std::make_shared<DrawableFrameRate>(widgetArea);
	AddGameObjectToScene(drawableFrameRate);	
}

void LevelManager::CreatePlayerStats()
{
	const auto lastRow = level->NumRows;
	const int lastColumn = level->NumCols;
	auto joinRects = [=](const bool vertically, std::initializer_list<SDL_Rect> rects)-> SDL_Rect
	{
		SDL_Rect result {};
		result.x = rects.begin()->x;
		result.y = rects.begin()->y;
		for(const auto& [x, y, w, h] : rects)
		{			
			!vertically ? result.w += w : result.w = w;
			vertically ? result.h += h : result.h = h;	
		}
		return result;
	};
	
	const auto pointsRoom = level->GetRoom(lastRow,lastColumn);
	const auto healthRoom = level->GetRoom(lastRow,1);
	const auto amount = pointsRoom->InnerBounds.h / 2;
	pointsRoom->InnerBounds.h /= 2;
	healthRoom->InnerBounds.h /= 2;
	pointsRoom->InnerBounds.y += amount/2;
	healthRoom->InnerBounds.y += amount/2;

	playerHealth = make_shared<DrawableText>(joinRects(false, {healthRoom->InnerBounds}), std::to_string(player->GetHealth()), SDL_Color {255,0,0,0});
	playerPoints = make_shared<DrawableText>(joinRects(false, {pointsRoom->InnerBounds}), std::to_string(player->GetPoints()), SDL_Color {0,0,255,0});
	AddGameObjectToScene(playerHealth);
	AddGameObjectToScene(playerPoints);
}

void LevelManager::CreateHud(const std::vector<std::shared_ptr<Room>>& rooms, const std::shared_ptr<Player>& inPlayer)
{
	constexpr auto firstRow = 1;
	constexpr auto firstCol = 1;

	// Place the hud in top left of screen
	const auto hudPosition = level->GetRoom(firstRow, firstCol)->GetCenter(inPlayer->GetWidth(), inPlayer->GetHeight());
	const auto hudAsset = GetAsset("hudspritesheet");

	// build hud
	hudItem = GameObjectFactory::Get().BuildStaticSprite(hudAsset, hudPosition);

	// add to scene
	InitializeHudItem(hudItem);
	AddGameObjectToScene(hudItem);
}

void LevelManager::InitializeHudItem(const std::shared_ptr<StaticSprite>& hudItem)
{
	hudItem->LoadSettings();
	hudItem->SubscribeToEvent(PlayerMovedEventTypeEventId);
}

void LevelManager::AddGameObjectToScene(const std::shared_ptr<GameObject>& gameObject) { eventManager->RaiseEvent(std::dynamic_pointer_cast<Event>(eventFactory->CreateAddToSceneEvent(gameObject)), this); }

void LevelManager::CreateAutoLevel()
{
	ResourceManager::Get()->IndexResourceFile(); 
	RemoveAllGameObjects();
	
	const auto _ = Get()->ChangeLevel(1);
		
	level = std::make_shared<Level>();
	level->Load(); // construct a level

	InitializeRooms(level->Rooms);
	CreatePlayer(level->Rooms, GetAsset("edge_player")->Uid);
	CreateAutoPickups(level->Rooms);
	CreateHud(level->Rooms, player);
}

void LevelManager::InitializePlayer(const std::shared_ptr<Player>& inPlayer, const std::shared_ptr<SpriteAsset>&spriteAsset) const
{
	inPlayer->SetMoveStrategy(std::make_shared<GameObjectMoveStrategy>(inPlayer, inPlayer->CurrentRoom));
	inPlayer->SetTag(constants::PlayerTag);
	inPlayer->LoadSettings();
	inPlayer->SetSprite(AnimatedSprite::Create(inPlayer->Position, spriteAsset));

	// We keep a reference to track of the player globally
	GameData::Get()->player = inPlayer;
}

void LevelManager::InitializeAutoPickups(const std::vector<std::shared_ptr<Pickup>>& inPickups)
{
	for (const auto& pickup : inPickups)
	{		
		pickup->StringProperties["value"] = "1"; // The value of each pickup if 1 point but this value could be read from config
		AddGameObjectToScene(pickup);
	}
}

void LevelManager::InitializeRooms(const std::vector<std::shared_ptr<Room>>& rooms)
{
	for (const auto& room : rooms)
	{
		room->LoadSettings();
		room->SubscribeToEvent(PlayerMovedEventTypeEventId);
		room->SubscribeToEvent(SettingsReloadedEventId);

		AddGameObjectToScene(room);
	}
}

bool LevelManager::ChangeLevel(const int levelNum) const
{
	bool changedLevel = false;
	try
	{
		gameCommands->RaiseChangedLevel(false, static_cast<short>(levelNum));
		changedLevel = true;
	}
	catch (exception &e)
	{
		std::stringstream message;
		message << "Could not start level reason="<< e.what() << ", level=" << std::to_string(levelNum);
		LogMessage(message.str());		
	}
	return changedLevel;
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