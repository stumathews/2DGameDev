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
#include <filesystem>

using namespace tinyxml2;
using namespace std;
using namespace gamelib;

Level::Level(const std::string& filename): NumCols(0), NumRows(0), ScreenWidth(0), ScreenHeight(0), isAutoLevel(false)
{
	FileName = filename;
	isAutoPopulatePickups = true;
}

Level::Level()
{
	isAutoLevel = true;	
	isAutoPopulatePickups = true;
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
		Rooms = RoomGenerator(static_cast<int>(ScreenWidth), static_cast<int>(ScreenHeight), NumRows, NumCols,
		                      SettingsManager::Get()->GetBool("grid", "removeSidesRandomly")).Generate();
		return; 
	}

	XMLDocument doc;

	doc.LoadFile(FileName.c_str());

	if (doc.ErrorID() == 0)
	{		
		auto* scene = doc.FirstChildElement("level");
		NumCols = std::strtol(scene->ToElement()->Attribute("cols"), nullptr, 0);
		NumRows = std::strtol(scene->ToElement()->Attribute("rows"), nullptr, 0);

		auto autoPopulatePickups = scene->ToElement()->Attribute("autoPopulatePickups");
		if (autoPopulatePickups != nullptr) 
		{
			string strToTransform = string(autoPopulatePickups);
			std::transform(strToTransform.begin(), strToTransform.end(), strToTransform.begin(), ::toupper);
			isAutoPopulatePickups = strToTransform == "TRUE";
		}
		
		ScreenWidth = SettingsManager::Get()->GetInt("global", "screen_width");
		ScreenHeight = SettingsManager::Get()->GetInt("global", "screen_height");

		// List of Rooms generated
		
		for (auto roomNode = scene->FirstChild(); roomNode; roomNode = roomNode->NextSibling()) //  <room ...>
		{
			auto roomElement = roomNode->ToElement();
			auto number = std::strtol(roomElement->ToElement()->Attribute("number"), nullptr, 0);
			auto top = string(roomElement->ToElement()->Attribute("top"));
			auto right = string(roomElement->ToElement()->Attribute("right"));
			auto bottom = string(roomElement->ToElement()->Attribute("bottom"));
			auto left = string(roomElement->ToElement()->Attribute("left"));

			auto row = number / NumCols; // row for this roomNumber
			auto rowCol0 = row * NumCols;
			auto col = number - rowCol0; // col for this roomNumber

			const auto square_width = ScreenWidth / NumCols;
			const auto square_height = ScreenHeight / NumRows;
			const auto roomName = string("Room") + std::to_string(number);

			auto room = std::make_shared<Room>(roomName, "Room", number, col * square_width, row * square_height,
			                                   square_width, square_height, false);
			
			auto SetWall = [&](const std::string& isSideVisible, const Side side, const std::shared_ptr<Room>& inRoom) -> void
			{
				if (isSideVisible == "True")
				{
					inRoom->AddWall(side);
				}
				else
				{
					inRoom->RemoveWall(side);
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
					
					if (gameObject->Type == "Player")
					{
						Player1 = dynamic_pointer_cast<Player>(gameObject);
					}
					if (gameObject->Type == "Pickup")
					{
						if (!IsAutoPopulatePickups())
						{
							auto pickup = dynamic_pointer_cast<Pickup>(gameObject);
							Pickups.push_back(pickup);
						}
					}
				}
			}

			Rooms.push_back(room);
		}

		Rooms::ConfigureRooms(NumRows, NumCols, Rooms);

		LevelManager::Get()->InitializePickups(Pickups);
	}
}

shared_ptr<GameObject> Level::ParseObject(XMLNode* pObject, const std::shared_ptr<Room>& room) const
{
	const auto attributes = GetNodeAttributes(pObject);
	string name = attributes.at("name");
	string type = attributes.at("type");
	const auto resourceId = stoi(attributes.at("resourceId"));

	shared_ptr<GameObject> gameObject;
		
	if (type == "Player" || type == "Pickup")
	{
		const auto spriteAsset = dynamic_pointer_cast<SpriteAsset>(ResourceManager::Get()->GetAssetInfo(resourceId));
		const auto assetDimensions = spriteAsset->Dimensions;		
		const auto positionInRoom = room->GetCenter(assetDimensions.GetWidth(), assetDimensions.GetHeight());
		auto animatedSprite = GameObjectFactory::Get().BuildSprite(name, type, spriteAsset, coordinate<int>(positionInRoom.GetX(), positionInRoom.GetY()), true);
			
		if (type == "Player")
		{
			const auto player = std::make_shared<Player>(name, type, room, assetDimensions.GetWidth(),
			                                             assetDimensions.GetHeight(), "playerNickName");
			LevelManager::Get()->InitializePlayer(player, spriteAsset);

			gameObject = player;
		}

		if (type == "Pickup")
		{
			const auto pickup = std::make_shared<Pickup>(name, type, positionInRoom.GetX(), positionInRoom.GetY(),
			                                             assetDimensions.GetWidth(), assetDimensions.GetHeight(),
			                                             true, room->GetRoomNumber());
			ResourceManager::Get()->IndexResourceFile();
			pickup->stringProperties["assetName"] = spriteAsset->name;
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

void Level::ParseProperty(XMLNode* pObjectChild, const shared_ptr<GameObject>& go)
{
	const auto attributes = GetNodeAttributes(pObjectChild);
	const auto name = attributes.at("name");
	const auto value = attributes.at("value");
	go->stringProperties[name] = value;
}
