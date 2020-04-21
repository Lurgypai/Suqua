#pragma once
#include <string>
#include "player.h"
#include "gamemode.h"
#include "network.h"

#include "../client/Client.h"
#include "../editor/EditableSystem.h"
#include "../player/ClientPlayerSystem.h"
#include "../graphics/PaletteManager.h"

class Game {
public:
	Game();
	void startOfflineGame();
	void startOnlineGame();
	void startStageEditor(const std::string & filePath);
	void loadStage(const std::string& stageName);

	//client side time
	Time_t tick;
	Client client;
	PhysicsSystem physics;
	CombatSystem combat;
	WeaponManager weapons;
	ClimbableSystem climbables;
	EditableSystem editables;
	PlayerManager players;
	ClientPlayerSystem clientPlayers;
	PaletteManager palettes;
	SpawnSystem spawns;
	DominationMode mode;
	OnlineSystem online;

	int playerCamId;
	int debugCamId;
	int editorCamId;

	EntityId getPlayerId();
	const Stage& getStage() const;

	EntityId testText;
private:
	Stage stage;
	EntityId playerId;
};

/*
the game is getting a hand in rendering now, as it needs to handle the camera
as renderables are loaded in, store them in "render groups", which are combinations of two things, a vector holding all the things that need to be rendered, and a camera to render them with.
when its time to render, loop through all of the "render groups" and set the camera and render.
*/