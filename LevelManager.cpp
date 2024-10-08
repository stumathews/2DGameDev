#include "LevelManager.h"
#include "common/Common.h"
#include "common/constants.h"
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
#include "utils/Utils.h"
#include <events/ReliableUdpPacketLossDetectedEvent.h>

#include "GameDataManager.h"

using namespace gamelib;
using namespace std;
using namespace ExpectationLib;

bool LevelManager::Initialize()
{
	if(initialized) { return true; }

	verbose =  GetBoolSetting("global", "verbose");
	disableCharacters = GetBoolSetting("global", "disableCharacters");

	GameData::Get()->IsNetworkGame = GetBoolSetting("global", "isNetworkGame");
	GameData::Get()->IsGameDone = false;
	GameData::Get()->IsNetworkGame = false;
	GameData::Get()->CanDraw = true;

	eventManager = EventManager::Get();
	eventFactory = EventFactory::Get();
	gameCommands = std::make_shared<GameCommands>();
	inputManager = std::make_shared<InputManager>(gameCommands, verbose);

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
	eventManager->SubscribeToEvent(NetworkTrafficReceivedEventId, this);
	eventManager->SubscribeToEvent(ReliableUdpPacketReceivedEventId, this);
	eventManager->SubscribeToEvent(ReliableUdpCheckSumFailedEventId, this);
	eventManager->SubscribeToEvent(ReliableUdpPacketLossDetectedEventId, this);
	eventManager->SubscribeToEvent(ReliableUdpAckPacketEventId, this);
	eventManager->SubscribeToEvent(ReliableUdpPacketRttCalculatedEventId, this);

	InitializeClientGameStatePusher();
	InitializeStatisticsCapturing();

	return initialized = true;
}


ListOfEvents LevelManager::HandleEvent(const std::shared_ptr<Event>& evt, const unsigned long inDeltaMs)
{
	if(evt->Id.PrimaryId == LevelChangedEventTypeEventId.PrimaryId) { OnLevelChanged(evt); }
	if(evt->Id.PrimaryId == UpdateProcessesEventId.PrimaryId) { processManager.UpdateProcesses(inDeltaMs); }
	if(evt->Id.PrimaryId == InvalidMoveEventId.PrimaryId) { gameCommands->InvalidMove();}
	if(evt->Id.PrimaryId == NetworkPlayerJoinedEventId.PrimaryId) { OnNetworkPlayerJoined(evt);}
	if(evt->Id.PrimaryId == StartNetworkLevelEventId.PrimaryId) { OnStartNetworkLevel(evt); }
	if(evt->Id.PrimaryId == FetchedPickupEventId.PrimaryId) { OnFetchedPickup(evt); }
	if(evt->Id.PrimaryId == GameWonEventId.PrimaryId) { OnGameWon();}
	if(evt->Id.PrimaryId == PlayerCollidedWithEnemyEventId.PrimaryId) { OnEnemyCollision(evt);}
	if(evt->Id.PrimaryId == PlayerDiedEventId.PrimaryId) { OnPlayerDied(); }
	if(evt->Id.PrimaryId == PlayerCollidedWithPickupEventId.PrimaryId) { OnPickupCollision(evt); }
	if(evt->Id.PrimaryId == NetworkPlayerJoinedEventId.PrimaryId) { OnNetworkPlayerJoinedEvent(evt); }
	if(evt->Id.PrimaryId == NetworkTrafficReceivedEventId.PrimaryId) { OnNetworkTrafficReceivedEvent(evt); }
	if(evt->Id.PrimaryId == ReliableUdpPacketReceivedEventId.PrimaryId) { OnReliableUdpPacketReceivedEvent(evt); }
	if(evt->Id.PrimaryId == ReliableUdpCheckSumFailedEventId.PrimaryId) { OnReliableUdpCheckSumFailedEvent(evt); }
	if(evt->Id.PrimaryId == ReliableUdpPacketLossDetectedEventId.PrimaryId) { OnReliableUdpPacketLossDetectedEvent(evt); }
	if(evt->Id.PrimaryId == ReliableUdpAckPacketEventId.PrimaryId) { OnReliableUdpAckPacketEvent(evt); }
	if(evt->Id.PrimaryId == ReliableUdpPacketRttCalculatedEventId.PrimaryId) { OnReliableUdpPacketRttCalculatedEvent(evt); }
		
	return {};
}

std::shared_ptr<InputManager> LevelManager::GetInputManager()
{
	return inputManager;
}

void LevelManager::OnEnemyCollision(const std::shared_ptr<Event>& evt) 
{
	gameCommands->FetchedPickup();

	const auto collisionEvent = To<PlayerCollidedWithEnemyEvent>(evt);
	const auto& enemyHitPoints = collisionEvent->Enemy->StringProperties["HitPoint"];
	const auto msg = to_string(collisionEvent->Player->GetHealth());

	collisionEvent->Player->IntProperties["Health"] -= strtol(enemyHitPoints.c_str(), nullptr, 0);

	Logger::Get()->LogThis(msg);

	playerHealth->Text = msg;

	if(collisionEvent->Player->GetHealth() <= 0)
	{		
		eventManager->RaiseEvent(To<Event>(eventFactory->CreateGenericEvent(PlayerDiedEventId, GetSubscriberName())), this);
	}
}

void LevelManager::OnPickupCollision(const std::shared_ptr<Event>& evt) const
{
	const auto collisionEvent = To<PlayerCollidedWithPickupEvent>(evt);
	const auto& pickupValue = collisionEvent->Pickup->StringProperties["value"];
	const auto msg = to_string(collisionEvent->Player->GetPoints());

	collisionEvent->Player->IntProperties["Points"] += strtol(pickupValue.c_str(), nullptr, 0);
	
	playerPoints->Text = msg;

	Logger::Get()->LogThis(msg);
}

void LevelManager::OnFetchedPickup(const std::shared_ptr<Event>& evt) const
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
	auto _ = ChangeLevel(1);

	// Network games all start on level 1 for now
	CreateLevel(GetSetting("global", "level1FileName"));
}

string LevelManager::GetSetting(const std::string& section, const std::string& settingName)
{
	return SettingsManager::Get()->GetString(section, settingName);
}

int LevelManager::GetIntSetting(const std::string& section, const std::string& settingName)
{
	return SettingsManager::Get()->GetInt(section, settingName);
}

bool LevelManager::GetBoolSetting(const std::string& section, const std::string& settingName)
{
	return SettingsManager::Get()->GetBool(section, settingName);
}

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
	switch(To<SceneChangedEvent>(evt)->SceneId)
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
		std::make_shared<Action>([&](unsigned long deltaMs) { gameCommands->ToggleMusic(verbose); }));
	const auto b = std::static_pointer_cast<Process>(std::make_shared<Action>([&](unsigned long deltaMs)
	{
		AudioManager::Play(ResourceManager::Get()->GetAssetInfo(SettingsManager::String("audio", "win_music")));
	}));
	
	const auto c = std::static_pointer_cast<Process>(std::make_shared<DelayProcess>(5000));

	const auto d = std::static_pointer_cast<Process>(std::make_shared<Action>([&](unsigned long deltaMs)
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

void LevelManager::GetKeyboardInput(const unsigned long deltaMs) const
{
	inputManager->Sample(deltaMs);
}

void LevelManager::CreatePlayer(const vector<shared_ptr<Room>> &rooms, const int resourceId) 
{	
	const auto playerNickName = GameData::Get()->IsNetworkGame
	?  GetSetting("networking", "nickname")
	    : "Player1";

	player = CharacterBuilder::BuildPlayer(playerNickName, GetRandomRoom(rooms), resourceId, playerNickName);

	if(disableCharacters)
	{
		return;	
	}

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

	if(disableCharacters)
	{
		return;
	}

	for(auto i = 0; i < numPickups; i++)
	{
		const auto& randomRoom = GetRandomRoom(rooms);
		const auto pickupName = string("RoomPickup") + std::to_string(randomRoom->GetRoomNumber());
		
		std::shared_ptr<SpriteAsset> spriteAssert;

		// Place 3 sets evenly of each type of pickup	
		if(i < 1 * part)
		{
			spriteAssert = To<SpriteAsset>(ResourceManager::Get()->GetAssetInfo(GetSetting("pickup1", "assetName")));
		}
		else if(i >= 1 * part && i < 2 * part)
		{
			spriteAssert = To<SpriteAsset>(ResourceManager::Get()->GetAssetInfo(GetSetting("pickup2", "assetName")));
		}
		else if(i >= 2 * part)
		{
			spriteAssert = To<SpriteAsset>(ResourceManager::Get()->GetAssetInfo(GetSetting("pickup3", "assetName")));
		}

		auto pickup = CharacterBuilder::BuildPickup(pickupName, randomRoom, spriteAssert->Uid);
		
		pickups.push_back(pickup);
	}

	// Setup the pickups
	InitializeAutoPickups(pickups);
}

void LevelManager::AddScreenWidgets(const std::vector<std::shared_ptr<Room>>& rooms)
{
	AddGameObjectToScene(CreateHud(rooms, player));	
	AddGameObjectToScene(CreateDrawableFrameRate());

	playerHealth = CreateDrawablePlayerHealth();
	playerPoints = CreateDrawablePlayerPoints();	
	
	AddGameObjectToScene(playerHealth);
	AddGameObjectToScene(playerPoints);
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
	
	if (level->IsAutoPopulatePickups())
	{
		CreateAutoPickups(rooms);
	}

	AddScreenWidgets(rooms);
}

std::shared_ptr<DrawableFrameRate> LevelManager::CreateDrawableFrameRate()
{
	const int lastColumn = level->NumCols;

	constexpr auto firstRow = 1;
	auto widgetArea = &level->GetRoom(firstRow,lastColumn)->Bounds;

	// Add frame rate
	drawableFrameRate = std::make_shared<DrawableFrameRate>(widgetArea);
	return drawableFrameRate;
}

std::shared_ptr<DrawableText> LevelManager::CreateDrawablePlayerHealth() const
{
	const auto lastRow = level->NumRows;
	const auto healthRoom = level->GetRoom(lastRow,1);
	const auto amount = healthRoom->InnerBounds.h / 2;	
	auto colour = SDL_Color {255,0,0,0}; // Red

	healthRoom->InnerBounds.h /= 2;
	healthRoom->InnerBounds.y += amount/2;

	return make_shared<DrawableText>(healthRoom->InnerBounds, std::to_string(player->GetHealth()), colour);

}

std::shared_ptr<DrawableText> LevelManager::CreateDrawablePlayerPoints() const
{
	const auto lastRow = level->NumRows;
	const int lastColumn = level->NumCols;
	const auto pointsRoom = level->GetRoom(lastRow,lastColumn);
	const auto amount = pointsRoom->InnerBounds.h / 2;	
	auto colour = SDL_Color {0,0,255,0}; // Blue

	pointsRoom->InnerBounds.h /= 2;
	pointsRoom->InnerBounds.y += amount/2;

	return make_shared<DrawableText>(pointsRoom->InnerBounds, std::to_string(player->GetPoints()), colour);
}

std::shared_ptr<StaticSprite> LevelManager::CreateHud(const std::vector<std::shared_ptr<Room>>& rooms,
                                                      const std::shared_ptr<Player>& inPlayer)
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
	return hudItem;
}

void LevelManager::InitializeHudItem(const std::shared_ptr<StaticSprite>& hudItem)
{
	hudItem->LoadSettings();
	hudItem->SubscribeToEvent(PlayerMovedEventTypeEventId);
}

void LevelManager::AddGameObjectToScene(const std::shared_ptr<GameObject>& gameObject)
{
	eventManager->RaiseEvent(To<Event>(eventFactory->CreateAddToSceneEvent(gameObject)), this);
}

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

	AddScreenWidgets(level->Rooms);
}

void LevelManager::InitializePlayer(const std::shared_ptr<Player>& inPlayer, const std::shared_ptr<SpriteAsset>&spriteAsset) const
{
	inPlayer->SetMoveStrategy(std::make_shared<GameObjectMoveStrategy>(inPlayer, inPlayer->CurrentRoom));
	inPlayer->SetTag(gamelib::PlayerTag);
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
		room->Initialize();
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
	const auto networkPlayerJoinedEvent = To<NetworkPlayerJoinedEvent>(evt);
	
	// Add the player to our level... find a suitable position for it
}

Mix_Chunk* LevelManager::GetSoundEffect(const std::string& name)
{
	return AudioManager::ToAudioAsset(GetAsset(name))->AsSoundEffect();
}

std::shared_ptr<Asset> LevelManager::GetAsset(const std::string& name)
{
	return ResourceManager::Get()->GetAssetInfo(name);
}

shared_ptr<Level> LevelManager::GetLevel() { return level; }

inline std::string LevelManager::GetSubscriberName()
{
	return "level_manager";
}


void LevelManager::OnNetworkPlayerJoinedEvent(const std::shared_ptr<Event>& evt) const
{
	const auto joinEvent =  To<NetworkPlayerJoinedEvent>(evt);
	stringstream message;
	message << joinEvent->Player.GetNickName() << " joined.";
	Logger::Get()->LogThis(message.str());
}

void LevelManager::OnNetworkTrafficReceivedEvent(const std::shared_ptr<Event>& evt)
{
	const auto networkPlayerTrafficReceivedEvent = To<NetworkTrafficReceivedEvent>(evt);
	std::stringstream message;
	message 
		    << networkPlayerTrafficReceivedEvent->BytesReceived << " bytes of data received from " << networkPlayerTrafficReceivedEvent->Identifier  
		    << ". Message: \"" << networkPlayerTrafficReceivedEvent->Message << "\"";

	// how much data did we receie in this second?
	networkingStatistics.BytesReceived += networkPlayerTrafficReceivedEvent->BytesReceived;
			    
	Logger::Get()->LogThis(message.str());
}

void LevelManager::OnReliableUdpPacketReceivedEvent(const std::shared_ptr<Event>& evt)
{
	const auto rudpEvent = To<ReliableUdpPacketReceivedEvent>(evt);
	const auto rudpMessage = rudpEvent->ReceivedMessage;
	stringstream bundledSeqs;

	bundledSeqs << "(";
	for(int i = 0; i < rudpMessage->DataCount();i++)
	{			
		bundledSeqs << rudpMessage->Data()[i].Sequence;
		if(i < rudpMessage->DataCount()-1)
		{
			bundledSeqs << ",";
		}
	}		
	bundledSeqs << ")";

	std::stringstream message;
	message 
		<< "Received " << rudpMessage->Header.Sequence << ". Playload: " << bundledSeqs.str()
	    << " Sender acks: "
		<< BitFiddler<uint32_t>::ToString(rudpMessage->Header.LastAckedSequence);

	// write stats
	networkingStatistics.CountPacketsReceived++;
	networkingStatistics.CountAggregateMessagesReceived += rudpMessage->DataCount();
	
	Logger::Get()->LogThis(message.str());
}

void LevelManager::OnReliableUdpCheckSumFailedEvent(const std::shared_ptr<Event>& evt)
{
	const auto failedChecksumEvent = To<ReliableUdpCheckSumFailedEvent>(evt);
	const auto failedMessage = failedChecksumEvent->failedMessage;
	std::stringstream message;

	message << "Checksum failed for sequence " << failedMessage->Header.Sequence << ". Dropping packet.";

	// write stats
	networkingStatistics.VerificationFailedCount++;

	Logger::Get()->LogThis(message.str());
}


void LevelManager::OnReliableUdpPacketLossDetectedEvent(const std::shared_ptr<Event>& evt)
{
	const auto reliableUdpPacketLossDetectedEvent = To<ReliableUdpPacketLossDetectedEvent>(evt);
	const auto resendingMessage = reliableUdpPacketLossDetectedEvent->messageBundle;

	stringstream bundledSeqs;
	std::stringstream message;

	for(int i = static_cast<int>(resendingMessage->Data().size()) -1 ; i >= 0 ; i--)
	{			
		bundledSeqs << resendingMessage->Data()[i].Sequence;
		if(i < resendingMessage->DataCount()-1)
		{
			bundledSeqs << ",";
		}
	}		

	message << "Packet loss detected. Sequences " << bundledSeqs.str() << " were not acknowledged by receiver and will be resent with sending sequence "
		    << resendingMessage->Header.Sequence;

	// Track statistics
	networkingStatistics.CountPacketsLost++;

	Logger::Get()->LogThis(message.str());
}

void LevelManager::OnReliableUdpAckPacketEvent(const std::shared_ptr<Event>& evt)
{
	const auto reliableUdpPacketLossDetectedEvent = To<ReliableUdpAckPacketEvent>(evt);
	const auto ackMessage = reliableUdpPacketLossDetectedEvent->ReceivedMessage;

	std::stringstream message;

	message << "Acknowledgement " << (reliableUdpPacketLossDetectedEvent->Sent ? "sent: " : " received: ") << ackMessage->Header.Sequence;

	// how many acks did we receive
	networkingStatistics.CountAcks++;

	Logger::Get()->LogThis(message.str());

}

void LevelManager::OnReliableUdpPacketRttCalculatedEvent(const std::shared_ptr<gamelib::Event>& evt)
{
	const auto rttEvent = To<ReliableUdpPacketRttCalculatedEvent>(evt);

	// The last latency recorded is a smooth moving average considering last 3 packets
	networkingStatistics.AverageLatencySMA3 = rttEvent->Rtt.Sma3;
	networkingStatistics.RttMs = rttEvent->Rtt.Rtt;
}


void LevelManager::InitializeStatisticsCapturing()
{
	statisticsIntervalTimer.SetFrequency(1000); // every second
	statisticsFile = make_shared<TextFile>("statistics.txt");
	std::stringstream header;
	header << "TimeSecs" << "\t"
	       << "BytesReceived" << "\t"
	       << "CountPacketsLost" << "\t"
	       << "CountPacketsReceived" << "\t"
	       << "AverageLatencyMsSma3" << "\t"
	       << "CountAcks" << "\t" 
	       << "VerificationFailedCount" << "\t" 
	       << "CountAggregateMessagesReceived" << "\t"
	       << "SendingRateMs"  << "\t"
	       << "SendingRatePs"  << "\t"
		   << "RttMs"
	       << std::endl;

	statisticsFile->Append(header.str(), false);

	// Create a process that will Write statistics every second
	const auto writeStatsProcess = std::static_pointer_cast<Process>(std::make_shared<Action>([&](const unsigned long deltaMs)
	{
		statisticsIntervalTimer.Update(deltaMs);
		statisticsIntervalTimer.DoIfReady([&]()
		{
			// Write the statistics to file
			const auto tSeconds = GameDataManager::Get()->GameWorldData.ElapsedGameTime / 1000;

			std::stringstream message;

			// CountPacketsLost, BandwidthUsed
			message
				<< tSeconds << "\t"
				<< networkingStatistics.BytesReceived  << "\t"
				<< networkingStatistics.CountPacketsLost  << "\t"
				<< networkingStatistics.CountPacketsReceived  << "\t"
				<< networkingStatistics.AverageLatencySMA3  << "\t"
				<< networkingStatistics.CountAcks  << "\t"
				<< networkingStatistics.VerificationFailedCount  << "\t"
				<< networkingStatistics.CountAggregateMessagesReceived << "\t"
				<< networkingStatistics.SendRateMs << "\t"
				<< networkingStatistics.SendRatePs << "\t"
				<< networkingStatistics.RttMs
				<< std::endl;

			// Write to file
			statisticsFile->Append(message.str(), false);

			// Reset statistics to zero.
			networkingStatistics.Reset();
		});
	}, false)); // we'll not mark ourselves as succeeded so the process manager will not remove it (emulates long running)
	processManager.AttachProcess(writeStatsProcess);
}

void LevelManager::InitializeClientGameStatePusher()
{
	const bool isGameServer = SettingsManager::Get()->GetBool("networking", "isGameServer");
	if(isGameServer) 
	{
		// We won't send out periodic pings to the game server if we are the game server
		return;
	}

	// Periodically ping the game server
	sendRateMs = SettingsManager::Get()->GetInt("gameStatePusher", "sendRateMs");
	increaseSendingRateEveryMs = SettingsManager::Get()->GetInt("gameStatePusher", "increaseSendingRateEveryMs");
	increaseSendingRateIncrementMs =  SettingsManager::Get()->GetInt("gameStatePusher", "increaseSendingRateIncrementMs"); 
	pingRateTimer.SetFrequency(sendRateMs);
	increasePingRateTimer.SetFrequency(increaseSendingRateEveryMs);

	networkingStatistics.SendRateMs = sendRateMs;

	const auto sendGameStateProcess = std::static_pointer_cast<Process>(std::make_shared<Action>([&](const unsigned long deltaMs)
	{
		pingRateTimer.Update(deltaMs);
		increasePingRateTimer.Update(deltaMs);

		// increase ping rate frequency
		increasePingRateTimer.DoIfReady([&]()
		{
			// only increase rate if we can (can't reduce sending interval by an amount greater than what it is now)
			if(sendRateMs > increaseSendingRateIncrementMs)
			{
				// increase sending rate by decreasing intervals between sends
				sendRateMs -= increaseSendingRateIncrementMs;

				// update sending rate to be written to statistics
				networkingStatistics.SendRateMs = sendRateMs;
				networkingStatistics.SendRatePs = 1000 / sendRateMs;
			}
			pingRateTimer.SetFrequency(sendRateMs);
		});

		// ping
		pingRateTimer.DoIfReady([=]()
		{
			auto elapsedTime = GameDataManager::Get()->GameWorldData.ElapsedGameTime;
			GameCommands::PingGameServer(elapsedTime);
		});
	}, false));

	processManager.AttachProcess(sendGameStateProcess);
}


LevelManager* LevelManager::Get() { if (instance == nullptr) { instance = new LevelManager(); } return instance; }
LevelManager::~LevelManager()
{
	instance = nullptr;
}
LevelManager* LevelManager::instance = nullptr;