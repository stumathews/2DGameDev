#pragma once

enum EventNumber
{
	CollidedWithEnemyEvent = 2000,
	FetchedPickup,
	Fire,
	GameWon,
	SettingsReloaded,
	GenerateNewLevel,
	InvalidMove
};

const static EventId FireEventId(Fire, "Fire");
const static EventId FetchedPickupEventId(FetchedPickup, "FetchedPickup");
const static EventId GameWonEventId(GameWon, "GameWon");
const static EventId SettingsReloadedEventId(SettingsReloaded, "SettingsReloaded");
const static EventId InvalidMoveEventId(InvalidMove, "InvalidMove");
