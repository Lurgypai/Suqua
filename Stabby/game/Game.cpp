#include <fstream>
#include <filesystem>

#include "EntityBaseComponent.h"
#include "ControllerComponent.h"
#include "RectDrawable.h"
#include "TextDrawable.h"
#include "PositionComponent.h"
#include "GLRenderer.h"
#include "DebugIO.h"
#include "MenuButtonComponent.h"
#include "MenuTextBoxComponent.h"
#include "MenuGridComponent.h"

#include "nlohmann/json.hpp"
#include "../graphics/PlayerGC.h"
#include "../graphics/CapturePointGC.h"
#include "../player/ClientPlayerComponent.h"
#include "../player/OnlinePlayerLC.h"
#include "../graphics/camera/PlayerCam.h"
#include "../graphics/StatBarComponent.h"
#include "../graphics/HealthReader.h"
#include "../graphics/StaminaReader.h"
#include "../sounds/PlayerSFX.h"
#include "../graphics/TeamPointsReader.h"

#include "Game.h"

using json = nlohmann::json;

Game::Game() :
	tick{0},
	gameState{GameState::main_menu},
	physics{},
	combat{},
	clientPlayers{&physics, &combat},
	weaponMenuOpen{false},
	client{ tick },
	sessionGuided{false},
	respawner{&spawns}
{
	clientSpawner.setClient(&client);
}

void Game::startMainMenu() {
	if (EntitySystem::Contains<EntityBaseComponent>()) {
		for (auto& entity : EntitySystem::GetPool<EntityBaseComponent>()) {
			entity.isDead = true;
		}
	}

	mainMenu = menus.makeMenu();
	auto& menu = menus.getMenu(mainMenu);
	menu.addMenuEntry(MenuEntryType::button, "start", AABB{ {278, 305}, {100, 25} });
	menu.addMenuEntry(MenuEntryType::text_box, "name", AABB{ {295, 241}, {100, 20} });
	menu.addMenuEntry(MenuEntryType::text_box, "ip_address", AABB{ {295, 262}, {100, 20} });
	menu.addMenuEntry(MenuEntryType::text_box, "port", AABB{ {295, 283}, {100, 20} });

	for (auto& button : menu.getButtons()) {
		EntityId id = button;
		EntitySystem::MakeComps<RenderComponent>(1, &id);
		RenderComponent* render = EntitySystem::GetComp<RenderComponent>(id);
		MenuButtonComponent* menuButton = EntitySystem::GetComp<MenuButtonComponent>(id);

		render->loadDrawable<RectDrawable>();
		auto* drawable = render->getDrawable<RectDrawable>();
		drawable->shape = menuButton->getBoundingBox();
		drawable->c = { 1.0, 1.0, 1.0, 1.0 };
		drawable->depth = 0.0;

		renderGroups["main_menu"].entities.push_back(button);
	}
	
	
	for (auto& textBox : menu.getTextBoxes()) {

		EntityId id = textBox;
		EntitySystem::MakeComps<RenderComponent>(1, &id);
		RenderComponent* render = EntitySystem::GetComp<RenderComponent>(id);

		render->loadDrawable<TextDrawable>();
		auto* drawable = render->getDrawable<TextDrawable>();
		drawable->color = { 1, 1, 1, 1 };
		drawable->anti_alias = true;
		drawable->font.loadDataFile("suqua/fonts/consolas.fnt");
		drawable->scale = {.5, .5};

		renderGroups["main_menu"].entities.push_back(textBox);
	}
	

	EntityId mainMenuBG;
	EntitySystem::GenEntities(1, &mainMenuBG);
	EntitySystem::MakeComps<RenderComponent>(1, &mainMenuBG);
	RenderComponent* mainMenuRender = EntitySystem::GetComp<RenderComponent>(mainMenuBG);
	mainMenuRender->loadDrawable<Sprite>("main_menu");
	EntitySystem::GetComp<PositionComponent>(mainMenuBG)->pos = { 0 , 0 };
	renderGroups["main_menu"].entities.push_back(mainMenuBG);
}

void Game::startOfflineGame(bool sessionGuided_) {
	for (auto& entity : EntitySystem::GetPool<EntityBaseComponent>()) {
		entity.isDead = true;
	}
	menus.getMenu(mainMenu).clear();
	loadWeaponMenu();
	loadRespawnMenu();
	editables.isEnabled = false;
	
	std::ifstream settingsFile{ "settings.json" };
	json settings{};
	settingsFile >> settings;
	std::string stageName = settings["stage"];

	loadStage(stageName);

	playerId = players.makePlayer(weapons);
	makePlayerGFX(playerId);

	EntitySystem::GetComp<PlayerLC>(playerId)->chooseSpawn();

	mode.load(&spawns, 2, 1, 100000);

	auto spawnables = stage.getSpawnables();
	EntitySystem::MakeComps<CapturePointGC>(spawnables.size(), spawnables.data());

	gameState = GameState::offline;

	for (auto& stageAsset : stage.getRenderables()) {
		renderGroups["gameplay"].entities.push_back(stageAsset);
	}
	for (auto& capturePoint : stage.getSpawnables()) {
		renderGroups["gameplay"].entities.push_back(capturePoint);
	}

	sessionGuided = sessionGuided_;

	if (sessionGuided) {
		session.unserialize("session");
		EntitySystem::MakeComps<ClientPlayerComponent>(1, &playerId);

		client.setPlayer(playerId);
		client.setWeaponManager(weapons);
		client.setClientPlayerSystem(&clientPlayers);
		client.setOnlineSystem(&online);
		client.setMode(&mode);
		client.setSpawns(&spawns);
		client.setSessionSystem(&session);

		SessionSystem::NetworkEvent netEvent = session.getNetworkEvents().front();

		WelcomePacket packet{};
		std::memcpy(&packet, netEvent.enetEvent.packet->data, netEvent.enetEvent.packet->dataLength);
		packet.unserialize();

		online.registerOnlineComponent(playerId, packet.netId);
		

		tick = session.getCurrTick();
	}

	loadInGameUI();
}

void Game::startOnlineGame() {
	for (auto& entity : EntitySystem::GetPool<EntityBaseComponent>()) {
		entity.isDead = true;
	}
	menus.getMenu(mainMenu).clear();
	loadWeaponMenu();
	loadRespawnMenu();
	editables.isEnabled = false;

	std::ifstream settingsFile{ "settings.json" };
	json settings{};
	settingsFile >> settings;
	std::string stageName = settings["stage"];
	std::string address = settings["ip"];
	std::string name = settings["name"];
	int port = settings["port"];

	loadStage(stageName);

	playerId = players.makePlayer(weapons);
	EntitySystem::MakeComps<ClientPlayerComponent>(1, &playerId);
	EntitySystem::GetComp<NameTagComponent>(playerId)->nameTag = name;
	makePlayerGFX(playerId);

	client.setPlayer(playerId);
	client.setWeaponManager(weapons);
	client.setClientPlayerSystem(&clientPlayers);
	client.setOnlineSystem(&online);
	client.setMode(&mode);
	client.setSpawns(&spawns);
	client.setSessionSystem(&session);

	client.connect(address, port);

	gameState = GameState::online;

	for (auto& stageAsset : stage.getRenderables()) {
		renderGroups["gameplay"].entities.push_back(stageAsset);
	}

	loadInGameUI();
}

void Game::startStageEditor(const std::string & filePath) {
	for (auto& entity : EntitySystem::GetPool<EntityBaseComponent>()) {
		entity.isDead = true;
	}
	renderGroups.clear();
	menus.getMenu(mainMenu).clear();

	editables.isEnabled = true;
	editables.load(filePath);

	gameState = GameState::stage_editor;

	renderGroups["editor"].entities.push_back(editables.getStageImage());
}

void Game::loadStage(const std::string& stageName) {
	stage = Stage(stageName, spawns);
	stage.loadGraphics();
}

void Game::loadCameras(int viewWidth, int viewHeight) {
	PlayerCam playerCam{ viewWidth, viewHeight };
	Camera editorCamera{ Vec2f{0.0f, 0.0f}, Vec2f{static_cast<float>(viewWidth), static_cast<float>(viewHeight)}, 1.0 };
	Camera menuCamera{ Vec2f{0.0f, 0.0f}, Vec2f{static_cast<float>(viewWidth), static_cast<float>(viewHeight)}, 1.0 };
	playerCamId = GLRenderer::addCamera(playerCam);
	editorCamId = GLRenderer::addCamera(editorCamera);
	menuCamId = GLRenderer::addCamera(menuCamera);
	iGUICamId = GLRenderer::addCamera(menuCamera);
	respawnMenuCamId = GLRenderer::addCamera(menuCamera);

	editorCam = EditorCam{ editorCamId };

	addRenderGroup("main_menu", menuCamId);
	addRenderGroup("editor", editorCamId);
	addRenderGroup("gameplay", playerCamId);
	addRenderGroup("name_tags", playerCamId);
	addRenderGroup("i_gui", iGUICamId);
	addRenderGroup("respawn_menu", respawnMenuCamId);
}

void Game::loadTextures() {
	//misc
	GLRenderer::LoadTexture("images/none.png", "none");

	//font
	GLRenderer::LoadTexture("suqua/fonts/consolas_0.png", "test_font");

	//menus
	GLRenderer::LoadTexture("images/temp_main_menu.png", "main_menu");
	GLRenderer::LoadTexture("images/weapon_gui.png", "weapon_menu");
	GLRenderer::LoadTexture("images/igui.png", "in_game_gui");
	GLRenderer::LoadTexture("images/respawn_arrow.png", "respawn_arrow");
	
	//player
	GLRenderer::LoadTexture("images/stabbyman.png", "character");

	//stage
	namespace fs = std::filesystem;

	fs::path path{ "stage/" };
	for (auto& file : fs::directory_iterator(path)) {
		if (file.path().extension() == ".png") {
			std::string id = file.path().stem().string();
			GLRenderer::LoadTexture(file.path().string(), "stage::" + id);
		}
	}

	//weapons
	path = fs::path{ "attacks/asset/" };
	for (auto& file : fs::directory_iterator(path)) {
		if (file.path().extension() == ".png") {
			std::string id = file.path().stem().string();
			GLRenderer::LoadTexture(file.path().string(), "weapon::" + id);
		}
	}
}

void Game::loadSounds() {
	sound.setTargetCam(&GLRenderer::getCamera(playerCamId));
	sound.setMaxVolume(128 / 2);
	sound.setMaxProximity(300);

	sound.loadSound("sounds/player/hurt.wav", "player.hurt1");
	sound.loadSound("sounds/player/step.wav", "player.step1");
	sound.loadSound("sounds/player/land.wav", "player.land1");
	sound.loadSound("sounds/player/roll.wav", "player.roll1");
	sound.loadSound("sounds/player/roll2.wav", "player.roll2");
	sound.loadSound("sounds/player/swing.wav", "player.swing1");
	sound.loadSound("sounds/player/swing2.wav", "player.swing2");
	sound.loadSound("sounds/player/death.wav", "player.death1");
	sound.loadSound("sounds/player/jump.wav", "player.jump1");
}

void Game::loadInGameUI() {
	EntitySystem::GenEntities(1, &inGameUI.healthBar);
	EntitySystem::GenEntities(1, &inGameUI.staminaBar);
	EntitySystem::GenEntities(1, &inGameUI.bg);
	EntitySystem::GenEntities(1, &inGameUI.icon);
	EntitySystem::GenEntities(1, &inGameUI.team1Bar);
	EntitySystem::GenEntities(1, &inGameUI.team2Bar);

	EntitySystem::MakeComps<StatBarComponent>(1, &inGameUI.healthBar);
	StatBarComponent* healthBar = EntitySystem::GetComp<StatBarComponent>(inGameUI.healthBar);
	healthBar->setTarget(playerId);
	healthBar->setStatReader<HealthReader>();
	healthBar->fullSize = AABB{ {109, 9}, {100, 5} };

	EntitySystem::MakeComps<StatBarComponent>(1, &inGameUI.staminaBar);
	StatBarComponent* staminaBar = EntitySystem::GetComp<StatBarComponent>(inGameUI.staminaBar);
	staminaBar->setTarget(playerId);
	staminaBar->setStatReader<StaminaReader>();
	staminaBar->fullSize = AABB{ {109, 18}, {80, 3} };

	EntitySystem::MakeComps<RenderComponent>(1, &inGameUI.bg);
	RenderComponent* bg = EntitySystem::GetComp<RenderComponent>(inGameUI.bg);
	bg->loadDrawable<Sprite>("in_game_gui");
	bg->getDrawable<Sprite>()->setDepth(-0.5);

	CombatComponent* combat = EntitySystem::GetComp<CombatComponent>(playerId);

	EntitySystem::MakeComps<RenderComponent>(1, &inGameUI.icon);
	RenderComponent* icon = EntitySystem::GetComp<RenderComponent>(inGameUI.icon);
	icon->loadDrawable<Sprite>("weapon::" + combat->getAttack().getId() + "_icon");
	EntitySystem::GetComp<PositionComponent>(inGameUI.icon)->pos = { 7,6 };
	icon->getDrawable<Sprite>()->setDepth(-0.6);

	renderGroups["i_gui"].entities.push_back(inGameUI.healthBar);
	renderGroups["i_gui"].entities.push_back(inGameUI.staminaBar);
	renderGroups["i_gui"].entities.push_back(inGameUI.bg);
	renderGroups["i_gui"].entities.push_back(inGameUI.icon);

	EntitySystem::MakeComps<StatBarComponent>(1, &inGameUI.team1Bar);
	EntitySystem::MakeComps<StatBarComponent>(1, &inGameUI.team2Bar);

	EntitySystem::MakeComps<StatBarComponent>(1, &inGameUI.team1Bar);
	StatBarComponent* team1Bar = EntitySystem::GetComp<StatBarComponent>(inGameUI.team1Bar);
	team1Bar->setStatReader<TeamPointsReader>();
	team1Bar->getStatReader<TeamPointsReader>().targetMode = &mode;
	team1Bar->getStatReader<TeamPointsReader>().targetTeamId = 1;
	team1Bar->fullSize = AABB{ {210, 347}, {100, 5} };

	EntitySystem::MakeComps<StatBarComponent>(1, &inGameUI.team2Bar);
	StatBarComponent* team2Bar = EntitySystem::GetComp<StatBarComponent>(inGameUI.team2Bar);
	team2Bar->setStatReader<TeamPointsReader>();
	team2Bar->getStatReader<TeamPointsReader>().targetMode = &mode;
	team2Bar->getStatReader<TeamPointsReader>().targetTeamId = 2;
	team2Bar->flipped = true;
	team2Bar->fullSize = AABB{ {330, 347}, {100, 5} };

	renderGroups["i_gui"].entities.push_back(inGameUI.team1Bar);
	renderGroups["i_gui"].entities.push_back(inGameUI.team2Bar);

	EntitySystem::GenEntities(1, &winningTeamText);
	EntitySystem::MakeComps<RenderComponent>(1, &winningTeamText);
	RenderComponent* winningText = EntitySystem::GetComp<RenderComponent>(winningTeamText);
	winningText->loadDrawable<TextDrawable>();
	TextDrawable* text = winningText->getDrawable<TextDrawable>();
	text->depth = -0.5;
	text->color = { 1.0, 1.0, 1.0, 1.0 };
	text->font.loadDataFile("suqua/fonts/consolas.fnt");
	text->text = "";

	renderGroups.at("i_gui").entities.emplace_back(winningTeamText);
}

void Game::addRenderGroup(const std::string& tag, int camId) {
	renderGroups[tag] = { tag, camId, {} };
}

void Game::updatePlayerCamera() {
	static_cast<PlayerCam&>(GLRenderer::getCamera(playerCamId)).update(playerId);
}

void Game::loadNewPlayers() {
	if (!client.getNewPlayers().empty()) {
		auto size = client.getNewPlayers().size();

		std::vector<EntityId> entities;
		entities.resize(size);
		EntitySystem::GenEntities(entities.size(), entities.data());
		EntitySystem::MakeComps<OnlinePlayerLC>(entities.size(), entities.data());
		EntitySystem::MakeComps<OnlineComponent>(entities.size(), entities.data());
		EntitySystem::MakeComps<NameTagComponent>(entities.size(), entities.data());

		unsigned int i = 0;
		for(auto& netId : client.getNewPlayers()) {
			EntityId entity = entities[i];
			online.registerOnlineComponent(entity, netId);
			EntitySystem::GetComp<CombatComponent>(entity)->hurtboxes.emplace_back(Hurtbox{ Vec2f{ -2, -20 }, AABB{ {0, 0}, {4, 20} } });
			EntitySystem::GetComp<CombatComponent>(entity)->health = 100;
			EntitySystem::GetComp<CombatComponent>(entity)->setAttack("sword");
			EntitySystem::GetComp<CombatComponent>(entity)->stats = CombatStats{ 100, 2, 0, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f };
			EntitySystem::GetComp<CombatComponent>(entity)->ignoreDamage = true;
			PositionComponent* pos = EntitySystem::GetComp<PositionComponent>(entity);
			pos->pos = { 0, 0 };

			makePlayerGFX(entity);

			++i;
		}
	}
	client.clearNewPlayers();
}

void Game::loadNewCapturePoints() {
	for (const auto& packet : client.getMissingCapturePoints()) {
		bool spawnWasFound = false;
		for (auto& spawn : EntitySystem::GetPool<SpawnComponent>()) {
			if (spawn.getSpawnZone() == packet.zone) {
				EntityId id = spawn.getId();
				spawnWasFound = true;
				mode.createZone(id, packet.zone, packet.state.currTeamId, packet.state.totalCaptureTime, packet.state.remainingCaptureTime);

				CapturePointComponent* capturePoint = EntitySystem::GetComp<CapturePointComponent>(id);
				capturePoint->setState(packet.state);

				EntitySystem::MakeComps<CapturePointGC>(1, &id);

				renderGroups["gameplay"].entities.push_back(id);

				online.registerOnlineComponent(id, packet.netId);
			}
		}
		if (!spawnWasFound) {
			//unable to sync with server, throw an error
			throw std::exception{};
		}
	}
	client.clearMissingCapturePoints();
}

void Game::updateEditorCamera() {
	editorCam.update();
}

void Game::updateEditor() {
	editables.updateLogic(editorCamId);
	addNewEditables();
}

void Game::updateMainMenu() {
	auto& menu = menus.getMenu(mainMenu);
	menu.updateMenuEntries(menuCamId);

	std::ifstream settingsFile{ "settings.json" };
	json settings{};
	settingsFile >> settings;
	std::string address = settings["ip"];
	int port = settings["port"];
	std::string name = settings["name"];
	settingsFile.close();

	MenuResult r{};
	while (menu.pollResult(r)) {
		switch (r.type) {
		case MenuEntryType::button:
		{
			std::string response{ r.button.response };
			if (response == "start") {
				startOnlineGame();
			}
		}
		case MenuEntryType::text_box:
		{
			std::string response{ r.text_box.resposne };
			if (r.entryTag == "ip_address") {
				settings["ip"] = response;
				address = response;
			}
			else if (r.entryTag == "port") {
				try {
					int newPort = std::stoi(response);
					settings["port"] = newPort;
					port = newPort;
				}
				catch (std::invalid_argument e) {}
			}
			else if (r.entryTag == "name") {
				settings["name"] = response;
				name = response;
			}
		}
		}
	}

	std::ofstream out{ "settings.json" };
	out << settings;
	out.close();

	for (auto& textBoxId : menu.getTextBoxes()) {
		MenuTextBoxComponent* textBox = EntitySystem::GetComp<MenuTextBoxComponent>(textBoxId);
		RenderComponent* render = EntitySystem::GetComp<RenderComponent>(textBoxId);

		if (textBox->getTag() == "ip_address") {
			if (textBox->isActive())
				render->getDrawable<TextDrawable>()->text = textBox->getActiveText();
			else
				render->getDrawable<TextDrawable>()->text = address;
		}
		else if (textBox->getTag() == "port") {
			if (textBox->isActive())
				render->getDrawable<TextDrawable>()->text = textBox->getActiveText();
			else
				render->getDrawable<TextDrawable>()->text = std::to_string(port);
		}
		else if (textBox->getTag() == "name") {
			if (textBox->isActive())
				render->getDrawable<TextDrawable>()->text = textBox->getActiveText();
			else
				render->getDrawable<TextDrawable>()->text = name;
		}
	}
}

void Game::updateWeaponMenu() {
	CombatComponent* playerCombat = EntitySystem::GetComp<CombatComponent>(playerId);
	//if he's dead, open the menu and toggle it to not open again.
	//if he's alive, allow to open menu, and clear the menu
	if (!playerCombat->isAlive()) {
		if (shouldOpenWeaponMenu) {
			shouldOpenWeaponMenu = false;
			weaponMenuOpen = true;
			openWeaponMenu();
		}
	}
	else {
		shouldOpenWeaponMenu = true;
		weaponMenuOpen = false;
		renderGroups["main_menu"].entities.clear();
	}

	//when the menu is open, update
	if(weaponMenuOpen) {
		//position gfx
		auto& weaponMenu_ = menus.getMenu(weaponMenu);
		weaponMenu_.updateMenuEntries(menuCamId);
		MenuTextBoxComponent* text = EntitySystem::GetComp<MenuTextBoxComponent>(weaponMenu_.getMenuEntry("search_bar"));
		MenuGridComponent* grid = EntitySystem::GetComp<MenuGridComponent>(weaponMenu_.getMenuEntry("weapon_grid"));

		auto buttonBoxes = grid->generateButtonBoxes();
		auto buttonTags = grid->getCurrButtons();
		int i = 0;
		
		for (auto& pair : weaponIcons) {
			RenderComponent* render = EntitySystem::GetComp<RenderComponent>(pair.second);
			render->getDrawable<Sprite>()->setScale({ 0, 0 });
		}
		if (buttonTags.empty()) {
			if (!grid->prefix.empty()) {
				auto iter = weaponIcons.find(grid->prefix);
				if (iter != weaponIcons.end()) {
					auto& currButton = buttonBoxes[i];

					EntityId buttonImgId = weaponIcons[grid->prefix];
					RenderComponent* render = EntitySystem::GetComp<RenderComponent>(buttonImgId);
					PositionComponent* pos = EntitySystem::GetComp<PositionComponent>(buttonImgId);
					pos->pos = currButton.pos;
					render->getDrawable<Sprite>()->setScale({ 1, 1 });
				}
			}
		}
		else {
			for (auto buttonTag : buttonTags) {
				buttonTag = grid->prefix + buttonTag;
				auto& currButton = buttonBoxes[i];

				EntityId buttonImgId = weaponIcons[buttonTag];
				RenderComponent* render = EntitySystem::GetComp<RenderComponent>(buttonImgId);
				PositionComponent* pos = EntitySystem::GetComp<PositionComponent>(buttonImgId);
				pos->pos = currButton.pos;
				render->getDrawable<Sprite>()->setScale({ 1, 1 });
				++i;
			}
		}

		MenuResult r;
		std::string currQuery;
		while (weaponMenu_.pollResult(r)) {
			switch (r.type) {
			case MenuEntryType::text_box:
			{
				std::string response{ r.text_box.resposne };
				if (r.entryTag == "search_bar") {
					currQuery = response;
				}
			}
			case MenuEntryType::grid:
			{
				std::string response{ r.grid.response };
				if (r.entryTag == "weapon_grid") {
					if (weapons.hasWeapon(response)) {
						//close the menu
						weaponMenuOpen = false;
						renderGroups["main_menu"].entities.clear();


						if (EntitySystem::Contains<PlayerLC>() && EntitySystem::Contains<PlayerGC>()) {
							if (!client.getConnected()) {
								PlayerLC* player = EntitySystem::GetComp<PlayerLC>(playerId);
								PlayerGC* graphics = EntitySystem::GetComp<PlayerGC>(playerId);

								player->setWeapon(response);
							}
							else {
								OnlineComponent* online = EntitySystem::GetComp<OnlineComponent>(playerId);
								size_t size = response.size();

								WeaponChangePacket p;
								p.size = size;
								p.id = online->getNetId();
								p.serialize();
								char* data = static_cast<char*>(malloc(sizeof(WeaponChangePacket) + size));
								memcpy(data, &p, sizeof(WeaponChangePacket));
								memcpy(data + sizeof(WeaponChangePacket), response.data(), size);
								client.send(sizeof(WeaponChangePacket) + size, data);
								free(data);
							}
						}
					}
				}
			}
			}
		}

		for (auto& textBoxId : weaponMenu_.getTextBoxes()) {
			MenuTextBoxComponent* textBox = EntitySystem::GetComp<MenuTextBoxComponent>(textBoxId);
			RenderComponent* render = EntitySystem::GetComp<RenderComponent>(textBoxId);

			if (textBox->getTag() == "search_bar") { 
				if (textBox->isActive()) {
					render->getDrawable<TextDrawable>()->text = textBox->getActiveText();
					grid->prefix = textBox->getActiveText();
				}
				else {
					render->getDrawable<TextDrawable>()->text = currQuery;
					grid->prefix = currQuery;
				}
			}
		}
	}
}

void Game::updateInGameUI() {
	CombatComponent* combat = EntitySystem::GetComp<CombatComponent>(playerId);
	RenderComponent* iconRender = EntitySystem::GetComp<RenderComponent>(inGameUI.icon);
	Sprite* icon = iconRender->getDrawable<Sprite>();
	icon->texture_tag = "weapon::" + combat->getAttack().getId() + "_icon";

	for (auto& statBar : EntitySystem::GetPool<StatBarComponent>()) {
		statBar.update();
	}

	RenderComponent* winningText = EntitySystem::GetComp<RenderComponent>(winningTeamText);
	PositionComponent* pos = EntitySystem::GetComp<PositionComponent>(winningTeamText);
	TextDrawable* textDrawable = winningText->getDrawable<TextDrawable>();
	if (mode.getWinner()) {
		textDrawable->text = "Team " + std::to_string(mode.getWinner()) + " has won!";
		pos->pos = (Vec2f{ 640, 360 } - textDrawable->getBoundingBox().res).scale(0.5);
	}
	else {
		textDrawable->text = "";
	}
}

void Game::loadWeaponMenu() {
	//weapon menu
	weaponMenu = menus.makeMenu();
	auto& weaponMenu_ = menus.getMenu(weaponMenu);
	weaponMenu_.addMenuEntry(MenuEntryType::text_box, "search_bar", AABB{ {222, 71}, {182, 18} });
	weaponMenu_.addMenuEntry(MenuEntryType::grid, "weapon_grid", AABB{ {218, 87}, {190, 190} });
	MenuTextBoxComponent* weaponMenuTextBox = EntitySystem::GetComp<MenuTextBoxComponent>(weaponMenu_.getMenuEntry("search_bar"));
	weaponMenuTextBox->setCharLimit(21);

	MenuGridComponent* weaponMenuGrid = EntitySystem::GetComp<MenuGridComponent>(weaponMenu_.getMenuEntry("weapon_grid"));
	weaponMenuGrid->buttonRes = { 32, 32 };
	weaponMenuGrid->margins = { 5, 5 };
	auto& tags = weapons.getAttackTags();
	weaponMenuGrid->setButtons(tags);

	EntitySystem::GenEntities(1, &weaponMenuBG);
	EntitySystem::MakeComps<RenderComponent>(1, &weaponMenuBG);
	RenderComponent* weaponMenuRender = EntitySystem::GetComp<RenderComponent>(weaponMenuBG);
	weaponMenuRender->loadDrawable<Sprite>("weapon_menu");
	weaponMenuRender->getDrawable<Sprite>()->setDepth(-0.7);
	EntitySystem::GetComp<PositionComponent>(weaponMenuBG)->pos = { 0, 0 };

	for (auto& weapon : tags) {
		EntityId iconId;
		EntitySystem::GenEntities(1, &iconId);
		EntitySystem::MakeComps<RenderComponent>(1, &iconId);
		RenderComponent* render = EntitySystem::GetComp<RenderComponent>(iconId);
		render->loadDrawable<Sprite>("weapon::" + weapon + "_icon");
		render->getDrawable<Sprite>()->setScale({ 0, 0 });
		render->getDrawable<Sprite>()->setDepth(-1.0);
		weaponIcons.emplace(weapon, iconId);
	}

	for (auto& textBox : weaponMenu_.getTextBoxes()) {
		EntityId id = textBox;
		EntitySystem::MakeComps<RenderComponent>(1, &id);
		RenderComponent* render = EntitySystem::GetComp<RenderComponent>(id);

		render->loadDrawable<TextDrawable>();
		auto* drawable = render->getDrawable<TextDrawable>();
		drawable->color = { 1.0, 1.0, 1.0, 1.0 };
		drawable->anti_alias = true;
		drawable->font.loadDataFile("suqua/fonts/consolas.fnt");
		drawable->scale = { .5, .5 };
		drawable->depth = -0.8;
	}
}

void Game::openWeaponMenu() {
	renderGroups["main_menu"].entities.clear();
	renderGroups["main_menu"].entities.push_back(weaponMenuBG);

	Menu& menu = menus.getMenu(weaponMenu);
	for (auto& text : menu.getTextBoxes()) {
		renderGroups["main_menu"].entities.push_back(text);
	}

	for (auto& pair : weaponIcons) {
		renderGroups["main_menu"].entities.push_back(pair.second);
	}
}

void Game::loadRespawnMenu() {
	respawnMenu.arrowOpen = false;
	EntitySystem::GenEntities(1, &respawnMenu.leftArrow);
	EntitySystem::GenEntities(1, &respawnMenu.rightArrow);
	EntitySystem::GenEntities(1, &respawnMenu.counter);

	EntitySystem::MakeComps<RenderComponent>(1, &respawnMenu.leftArrow);
	EntitySystem::MakeComps<RenderComponent>(1, &respawnMenu.rightArrow);
	EntitySystem::MakeComps<RenderComponent>(1, &respawnMenu.counter);

	RenderComponent* leftArrowRender = EntitySystem::GetComp<RenderComponent>(respawnMenu.leftArrow);
	RenderComponent* rightArrowRender = EntitySystem::GetComp<RenderComponent>(respawnMenu.rightArrow);
	RenderComponent* counterRender = EntitySystem::GetComp<RenderComponent>(respawnMenu.counter);

	leftArrowRender->loadDrawable<AnimatedSprite>("respawn_arrow", Vec2i{-11, 20});
	rightArrowRender->loadDrawable<AnimatedSprite>("respawn_arrow", Vec2i{ 11, 20 });
	leftArrowRender->getDrawable<AnimatedSprite>()->setDepth(-0.3);
	rightArrowRender->getDrawable<AnimatedSprite>()->setDepth(-0.3);
	leftArrowRender->getDrawable<AnimatedSprite>()->addAnimation(0, 0, 1);
	rightArrowRender->getDrawable<AnimatedSprite>()->addAnimation(0, 0, 1);
	leftArrowRender->getDrawable<AnimatedSprite>()->setAnimation(0);
	rightArrowRender->getDrawable<AnimatedSprite>()->setAnimation(0);

	EntitySystem::GetComp<PositionComponent>(respawnMenu.leftArrow)->pos = {300, 170};
	EntitySystem::GetComp<PositionComponent>(respawnMenu.rightArrow)->pos = {340, 170};

	counterRender->loadDrawable<TextDrawable>();
	TextDrawable* text = counterRender->getDrawable<TextDrawable>();
	text->depth = -0.5;
	text->color = {1.0, 1.0, 1.0, 1.0};
	text->font.loadDataFile("suqua/fonts/consolas.fnt");
}

void Game::openRespawnMenu() {
	renderGroups["respawn_menu"].entities.clear();
	CombatComponent* combat = EntitySystem::GetComp<CombatComponent>(playerId);
	int spawnCount = 0;
	for (auto& spawn : EntitySystem::GetPool<SpawnComponent>()) {
		if (spawn.getTeamId() == combat->teamId) {
			++spawnCount;
			//as soon as we know there are enough spawns to open the menu
			if (spawnCount > 1) break;
		}
	}
	if (spawnCount > 1 && renderGroups["respawn_menu"].entities.empty()) {
		renderGroups["respawn_menu"].entities.push_back(respawnMenu.leftArrow);
		renderGroups["respawn_menu"].entities.push_back(respawnMenu.rightArrow);
		respawnMenu.arrowOpen = true;
	}
}

void Game::updateRespawnMenu() {
	PlayerLC* player = EntitySystem::GetComp<PlayerLC>(playerId);

	if (player->getState().state == State::dead && !player->shouldRespawn()) {
		if (!respawnMenu.counterVisible) {
			respawnMenu.counterVisible = true;
			renderGroups["respawn_menu"].entities.push_back(respawnMenu.counter);
			respawnMenu.counterVal = 10;
		}
		TextDrawable* text = EntitySystem::GetComp<RenderComponent>(respawnMenu.counter)->getDrawable<TextDrawable>();
		text->text = std::to_string(static_cast<int>(std::ceil(respawnMenu.counterVal * (1.0f - player->getRespawnProgress()))));
		const auto& box = text->getBoundingBox();
		EntitySystem::GetComp<PositionComponent>(respawnMenu.counter)->pos = { 320 - (box.res.x/2), 180-(box.res.y/2) };
	}
	else {
		respawnMenu.counterVisible = false;
		EntitySystem::GetComp<RenderComponent>(respawnMenu.counter)->getDrawable<TextDrawable>()->text = "";
	}

	if (player->shouldRespawn()) {
		if (!respawnMenu.arrowOpen) {
			openRespawnMenu();
		}
	}
	else {
		if (respawnMenu.arrowOpen) {
			respawnMenu.arrowOpen = false;
			renderGroups["respawn_menu"].entities.clear();
		}
	}

	ControllerComponent* playerCont = EntitySystem::GetComp<ControllerComponent>(playerId);
	if (playerCont->getController()[ControllerBits::LEFT]) {
		EntitySystem::GetComp<RenderComponent>(respawnMenu.leftArrow)->getDrawable<AnimatedSprite>()->setFrame(1);
	}
	else {
		EntitySystem::GetComp<RenderComponent>(respawnMenu.leftArrow)->getDrawable<AnimatedSprite>()->setFrame(0);
	}
	if (playerCont->getController()[ControllerBits::RIGHT]) {
		EntitySystem::GetComp<RenderComponent>(respawnMenu.rightArrow)->getDrawable<AnimatedSprite>()->setFrame(1);
	}
	else {
		EntitySystem::GetComp<RenderComponent>(respawnMenu.rightArrow)->getDrawable<AnimatedSprite>()->setFrame(0);
	}
}

void Game::addNewEditables() {
	for (const auto& id : editables.getNewEditables()) {
		renderGroups["editor"].entities.push_back(id);
	}
	editables.clearNewEditables();
}

void Game::makePlayerGFX(EntityId playerId_) {
	EntitySystem::MakeComps<PlayerGC>(1, &playerId_);
	EntitySystem::GetComp<RenderComponent>(playerId_)->loadDrawable<AnimatedSprite>("character", Vec2i{ 64, 64 });
	EntitySystem::GetComp<RenderComponent>(playerId_)->getDrawable<AnimatedSprite>()->setDepth(0.0);
	EntitySystem::GetComp<PlayerGC>(playerId_)->loadAnimations(weapons);

	EntitySystem::GetComp<PlayerGC>(playerId_)->loadNameTag();

	EntitySystem::MakeComps<PlayerSFX>(1, &playerId_);

	renderGroups["gameplay"].entities.push_back(playerId_);
	renderGroups["gameplay"].entities.push_back(EntitySystem::GetComp<PlayerGC>(playerId_)->getNameTageRenderId());
}

void Game::hideEnemyNametags() {
	CombatComponent* ourCombat = EntitySystem::GetComp<CombatComponent>(playerId);
	for (auto& combat : EntitySystem::GetPool<CombatComponent>()) {
		if (combat.teamId != ourCombat->teamId) {
			PlayerGC* otherPlayer = EntitySystem::GetComp<PlayerGC>(combat.getId());
			RenderComponent* otherNameTag = EntitySystem::GetComp<RenderComponent>(otherPlayer->getNameTageRenderId());
			otherNameTag->getDrawable<TextDrawable>()->text = "";
		}
	}
}

void Game::renderAll() {
	for (auto& pair : renderGroups) {
		GLRenderer::setCamera(pair.second.camId);
		for (auto& entity : pair.second.entities) {
			RenderComponent* comp = EntitySystem::GetComp<RenderComponent>(entity);
			if (comp) {
				render.draw(*comp);
			}
			else {
				//remove it
			}
		}
	}
}

void Game::playAll() {
	sound.playTriggeredSounds();
}

void Game::renderUI() {
	const std::string ids[] = {"i_gui", "respawn_menu", "main_menu"};
	for (auto& id : ids) {
		GLRenderer::setCamera(renderGroups[id].camId);
		for (auto& entity : renderGroups[id].entities) {
			RenderComponent* comp = EntitySystem::GetComp<RenderComponent>(entity);
			if (comp) {
				render.draw(*comp);
			}
			else {
				//remove it
			}
		}
	}
}

void Game::renderPlayArea() {
	const std::string ids[] = { "gameplay" };
	for (auto& id : ids) {
		GLRenderer::setCamera(renderGroups[id].camId);
		for (auto& entity : renderGroups[id].entities) {
			RenderComponent* comp = EntitySystem::GetComp<RenderComponent>(entity);
			if (comp) {
				render.draw(*comp);
			}
			else {
				//remove it
			}
		}
	}
}

const PlayerCam& Game::getPlayerCam() const {
	return static_cast<PlayerCam&>(GLRenderer::getCamera(playerCamId));
}

EntityId Game::getPlayerId() {
	return playerId;
}

const Stage& Game::getStage() const {
	return stage;
}

Game::GameState Game::getState() {
	return gameState;
}

bool Game::isSessionGuided() const {
	return sessionGuided;
}
