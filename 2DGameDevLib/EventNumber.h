#pragma once

enum EventNumber
{
	CollidedWithEnemy = 2000,
	FetchedPickup,
	Fire,
	GameWon,
	SettingsReloaded,
	GenerateNewLevel,
	InvalidMove,
	PLayerDied,
	PlayerCollidedWithPickup,
	EnemyMoved
};

const static EventId FireEventId(Fire, "Fire");
const static EventId FetchedPickupEventId(FetchedPickup, "FetchedPickup");
const static EventId GameWonEventId(GameWon, "GameWon");
const static EventId PlayerDiedEventId(PLayerDied, "PLayerDied");
const static EventId SettingsReloadedEventId(SettingsReloaded, "SettingsReloaded");
const static EventId InvalidMoveEventId(InvalidMove, "InvalidMove");
