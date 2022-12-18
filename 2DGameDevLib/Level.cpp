#include "Level.h"
#include "pch.h"
#include "Level.h"
#include "tinyxml2.h"
#include "Room.h"
#include <util/SettingsManager.h>
#include "Rooms.h"
#include "RoomGenerator.h"
#include <objects/GameObject.h>
#include <objects/GameObjectFactory.h>
#include <resource/ResourceManager.h>
#include "Player.h"
#include "pickup.h"
#include "../game/LevelManager.h"

using namespace tinyxml2;
using namespace std;
using namespace gamelib;

Level::Level(std::string filename) { FileName = filename; }

Level::Level()
{
	isAutoLevel = true;	
	NumRows = SettingsManager::Get()->GetInt("grid", "rows");
	NumCols = SettingsManager::Get()->GetInt("grid", "cols");
	ScreenWidth = SettingsManager::Get()->GetInt("global", "screen_width");
	ScreenHeight = SettingsManager::Get()->GetInt("global", "screen_height");
}

map<string, string> GetNodeAttributes(XMLNode* pAssetNode)
{
	map<string, string> attributes;
	if (pAssetNode)
	{

		// Nodes need to be converted to elements to access their attributes
		for (auto attribute = pAssetNode->ToElement()->FirstAttribute(); attribute; attribute = attribute->Next())
		{
			const auto* name = attribute->Name();
			const auto* value = attribute->Value();
			attributes[name] = value;
		}
	}
	return attributes;
}

void Level::Load()
{
	if (IsAutoLevel()) 
	{				
		Rooms = RoomGenerator(ScreenWidth, ScreenHeight, NumRows, NumCols, SettingsManager::Get()->GetBool("grid", "removeSidesRandomly")).Generate();
		return; 
	}

	tinyxml2::XMLDocument doc;

	doc.LoadFile(FileName.c_str());

	if (doc.ErrorID() == 0)
	{		
		auto* scene = doc.FirstChildElement("level");
		NumCols = static_cast<int>(std::atoi(scene->ToElement()->Attribute("cols")));
		NumRows = static_cast<int>(std::atoi(scene->ToElement()->Attribute("rows")));
		ScreenWidth = SettingsManager::Get()->GetInt("global", "screen_width");
		ScreenHeight = SettingsManager::Get()->GetInt("global", "screen_height");

		isAutoLevel = false;

		// List of Rooms generated
		
		for (auto roomNode = scene->FirstChild(); roomNode; roomNode = roomNode->NextSibling()) //  <room ...>
		{
			auto roomElement = roomNode->ToElement();
			auto number = static_cast<int>(std::atoi(roomElement->ToElement()->Attribute("number")));
			auto top = string(roomElement->ToElement()->Attribute("top"));
			auto right = string(roomElement->ToElement()->Attribute("right"));
			auto bottom = string(roomElement->ToElement()->Attribute("bottom"));
			auto left = string(roomElement->ToElement()->Attribute("left"));

			auto row = number / NumCols; // row for this roomNumber
			auto rowCol0 = row * NumCols;
			auto col = number - rowCol0; // col for this roomNumber

			const auto square_width = ScreenWidth / NumCols;
			const auto square_height = ScreenHeight / NumRows;

			auto room = std::shared_ptr<Room>(new Room(number, col * square_width, row * square_height, square_width, square_height, false));
			
			auto SetWall = [&](std::string isSideVisible, Side side, std::shared_ptr<Room> room) -> void
			{
				if (isSideVisible == "True")
				{
					room->AddWall(side);
				}
				else
				{
					room->RemoveWall(side);
				}
			};

			SetWall(right, Side::Right, room);
			SetWall(left, Side::Left, room);
			SetWall(top, Side::Top, room);
			SetWall(bottom, Side::Bottom, room);

			// Set room tag to room number
			room->SetTag(std::to_string(number));

			// <object name="xyz" type="abc" ...
			for (auto pRoomChild = roomNode->FirstChild(); pRoomChild; pRoomChild = pRoomChild->NextSibling())
			{
				string roomChildName = pRoomChild->Value();
				if (roomChildName == "object")
				{					
					auto gameObject = ParseObject(pRoomChild, room);
					
					if (gameObject->Type == "Player" || gameObject->GetGameObjectType() == GameObjectType::Player)
					{
						Player1 = dynamic_pointer_cast<Player>(gameObject);
					}
					if (gameObject->Type == "Pickup" || gameObject->GetGameObjectType() == GameObjectType::Pickup)
					{
						shared_ptr<Pickup> pickup = dynamic_pointer_cast<Pickup>(gameObject);

						Pickups.push_back(pickup);						
					}
				}
			}

			// Add to list of rooms in game
			Rooms.push_back(room);
		}

		Rooms::ConfigureRooms(NumRows, NumCols, Rooms);
	}
}

shared_ptr<GameObject> Level::ParseObject(tinyxml2::XMLNode* pObject, std::shared_ptr<Room> room)
{
	auto attributes = GetNodeAttributes(pObject);
	auto name = attributes.at("name");
	auto type = attributes.at("type");
	auto resourceId = stoi(attributes.at("resourceId"));

	shared_ptr<GameObject> gameObject;
		
	if (type == "Player" || type == "Pickup")
	{
		auto spriteAsset = dynamic_pointer_cast<SpriteAsset>(ResourceManager::Get()->GetAssetInfo(resourceId));
		auto colourKey = spriteAsset->GetColourKey();
		auto assetDimensions = spriteAsset->Dimensions;
		auto animatedSprite = GameObjectFactory::Get().BuildSprite(name, type, spriteAsset, coordinate<int>(0, 0), true, colourKey);
		const auto positionInRoom = room->GetCenter(assetDimensions.GetWidth(), assetDimensions.GetHeight());
			
		if (type == "Player")
		{
			auto player = shared_ptr<Player>(new Player(name, type, room, assetDimensions.GetWidth(), assetDimensions.GetHeight(), "playerNickName"));
			LevelManager::Get()->InitializePlayer(player, spriteAsset);

			gameObject = player;
		}

		if (type == "Pickup")
		{
			auto pickup = shared_ptr<Pickup>(new gamelib::Pickup(positionInRoom.GetX(), positionInRoom.GetY(), assetDimensions.GetWidth(), assetDimensions.GetHeight(), true, room->GetRoomNumber()));
			pickup->stringProperties["assetName"] = LevelManager::Get()->GetSetting("pickup1", "assetName");
			pickup->Initialize();
			LevelManager::Get()->InitializePickups(Pickups);

			gameObject = pickup;
		}
	}
	
	// Add properties to the game object
	for (auto pObjectChild = pObject->FirstChild(); pObjectChild; pObjectChild = pObjectChild->NextSibling())
	{
		string objectChildName = pObjectChild->Value();

		if (objectChildName == "property")
		{
			ParseProperty(pObjectChild, gameObject);
		}
	}
	return gameObject;
}

void Level::ParseProperty(tinyxml2::XMLNode* pObjectChild, shared_ptr<GameObject> go)
{
	auto attributes = GetNodeAttributes(pObjectChild);
	auto name = attributes.at("name");
	auto value = attributes.at("value");
	go->stringProperties[name] = value;
}
