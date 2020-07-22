#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include "RenderSystem.h"
#include "MenuSystem.h"

#include "player.h"
#include "gamemode.h"
#include "network.h"

#include "../client/Client.h"
#include "../editor/EditableSystem.h"
#include "../player/ClientPlayerSystem.h"
#include "../graphics/PaletteManager.h"
#include "../graphics/camera/EditorCam.h"
#include "../graphics/camera/PlayerCam.h"

class Game {
public:
	enum class GameState {
		online,
		offline,
		main_menu,
		pause_menu,
		stage_editor
	};

	Game();
	void startMainMenu();
	void startOfflineGame();
	void startOnlineGame();
	void startStageEditor(const std::string & filePath);
	void loadStage(const std::string& stageName);
	void loadCameras(int viewWidth, int viewHeight);
	void loadTextures();
	
	void updatePlayerCamera();
	void loadNewPlayers();
	void loadNewCapturePoints();
	void makePlayerGFX(EntityId playerId_);

	void updateEditorCamera();
	void updateEditor();

	void updateMainMenu();
	void updateWeaponMenu();

	void loadWeaponMenu();
	void openWeaponMenu();

	void renderAll(double gfxDelay);

	const PlayerCam& getPlayerCam() const;

	//client side time
	Time_t tick;
	RenderSystem render;
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
	MenuSystem menus;

	EntityId getPlayerId();
	const Stage& getStage() const;

	GameState getState();
private:

	Stage stage;
	EntityId playerId;
	GameState gameState;
	std::unordered_map<int, std::vector<EntityId>> renderGroups;

	int playerCamId;
	int debugCamId;
	int editorCamId;
	int menuCamId;

	EditorCam editorCam;

	MenuId mainMenu;
	MenuId weaponMenu;
	std::unordered_map<std::string, EntityId> weaponIcons;
	bool weaponMenuOpen;
	bool shouldOpenWeaponMenu;
	EntityId weaponMenuBG;
};

/*
the game is getting a hand in rendering now, as it needs to handle the camera
as renderables are loaded in, store them in "render groups", which are combinations of two things, a vector holding all the things that need to be rendered, and a camera to render them with.
when its time to render, loop through all of the "render groups" and set the camera and render.
*/