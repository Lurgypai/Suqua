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

#include "nlohmann/json.hpp"
#include "../graphics/PlayerGC.h"
#include "../graphics/CapturePointGC.h"
#include "../player/ClientPlayerComponent.h"
#include "../player/OnlinePlayerLC.h"
#include "../graphics/camera/PlayerCam.h"

#include "Game.h"

using json = nlohmann::json;

Game::Game() :
	gameState{GameState::main_menu},
	physics{},
	combat{},
	clientPlayers{&physics, &combat}
{}

void Game::startMainMenu() {
	if (EntitySystem::Contains<EntityBaseComponent>()) {
		for (auto& entity : EntitySystem::GetPool<EntityBaseComponent>()) {
			entity.isDead = true;
		}
		renderGroups.clear();
	}

	mainMenu = menus.makeMenu();
	auto& menu = menus.getMenu(mainMenu);
	menu.addMenuEntry(MenuEntryType::button, "start", AABB{ {278, 305}, {100, 25} });
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
		drawable->r = 1;
		drawable->g = 1;
		drawable->b = 1;

		renderGroups[menuCamId].push_back(button);
	}

	
	for (auto& textBox : menu.getTextBoxes()) {

		EntityId id = textBox;
		EntitySystem::MakeComps<RenderComponent>(1, &id);
		RenderComponent* render = EntitySystem::GetComp<RenderComponent>(id);
		MenuButtonComponent* menuButton = EntitySystem::GetComp<MenuButtonComponent>(id);

		render->loadDrawable<TextDrawable>();
		auto* drawable = render->getDrawable<TextDrawable>();
		drawable->setColor(1, 1, 1);
		drawable->anti_alias = true;
		drawable->font.loadDataFile("suqua/fonts/consolas.fnt");
		drawable->scale = {.5, .5};

		renderGroups[menuCamId].push_back(textBox);
	}
	

	EntityId mainMenuBG;
	EntitySystem::GenEntities(1, &mainMenuBG);
	EntitySystem::MakeComps<RenderComponent>(1, &mainMenuBG);
	RenderComponent* mainMenuRender = EntitySystem::GetComp<RenderComponent>(mainMenuBG);
	mainMenuRender->loadDrawable<Sprite>("main_menu");
	EntitySystem::GetComp<PositionComponent>(mainMenuBG)->pos = { 0 , 0 };
	renderGroups[menuCamId].push_back(mainMenuBG);
}

void Game::startOfflineGame() {
	for (auto& entity : EntitySystem::GetPool<EntityBaseComponent>()) {
		entity.isDead = true;
	}
	renderGroups.clear();
	menus.getMenu(mainMenu).clear();
	editables.isEnabled = false;
	
	std::ifstream settingsFile{ "settings.json" };
	json settings{};
	settingsFile >> settings;
	std::string stageName = settings["stage"];

	loadStage(stageName);

	playerId = players.makePlayer(weapons);
	makePlayerGFX(playerId);

	EntitySystem::GetComp<PlayerLC>(playerId)->chooseSpawn();

	mode.load(&spawns, 2, 1, 6000);

	auto spawnables = stage.getSpawnables();
	EntitySystem::MakeComps<CapturePointGC>(spawnables.size(), spawnables.data());

	gameState = GameState::offline;

	for (auto& stageAsset : stage.getRenderables()) {
		renderGroups[playerCamId].push_back(stageAsset);
	}
	for (auto& capturePoint : stage.getSpawnables()) {
		renderGroups[playerCamId].push_back(capturePoint);
	}
	renderGroups[playerCamId].push_back(playerId);
}

void Game::startOnlineGame() {
	for (auto& entity : EntitySystem::GetPool<EntityBaseComponent>()) {
		entity.isDead = true;
	}
	renderGroups.clear();
	menus.getMenu(mainMenu).clear();
	editables.isEnabled = false;

	std::ifstream settingsFile{ "settings.json" };
	json settings{};
	settingsFile >> settings;
	std::string stageName = settings["stage"];
	std::string address = settings["ip"];
	int port = settings["port"];

	loadStage(stageName);

	playerId = players.makePlayer(weapons);
	EntitySystem::MakeComps<ClientPlayerComponent>(1, &playerId);
	makePlayerGFX(playerId);

	client.setPlayer(playerId);
	client.setWeaponManager(weapons);
	client.setClientPlayerSystem(&clientPlayers);
	client.setOnlineSystem(&online);
	client.setMode(&mode);
	client.setSpawns(&spawns);

	client.connect(address, port);

	gameState = GameState::online;

	for (auto& stageAsset : stage.getRenderables()) {
		renderGroups[playerCamId].push_back(stageAsset);
	}
	renderGroups[playerCamId].push_back(playerId);
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

	renderGroups[editorCamId].push_back(editables.getStageImage());
	if (EntitySystem::Contains<EditableStageComponent>()) {
		for (auto& editableStageComponent : EntitySystem::GetPool<EditableStageComponent>()) {
			renderGroups[editorCamId].push_back(editableStageComponent.getId());
		}
	}
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

	editorCam = EditorCam{ editorCamId };
}

void Game::loadTextures() {
	//misc
	GLRenderer::LoadTexture("images/none.png", "none");

	//font
	GLRenderer::LoadTexture("suqua/fonts/consolas_0.png", "test_font");

	//menus
	GLRenderer::LoadTexture("images/temp_main_menu.png", "main_menu");
	
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

		unsigned int i = 0;
		for(auto& netId : client.getNewPlayers()) {
			EntityId entity = entities[i];
			online.registerOnlineComponent(entity, netId);
			EntitySystem::GetComp<CombatComponent>(entity)->hurtboxes.emplace_back(Hurtbox{ Vec2f{ -2, -20 }, AABB{ {0, 0}, {4, 20} } });
			EntitySystem::GetComp<CombatComponent>(entity)->health = 100;
			EntitySystem::GetComp<CombatComponent>(entity)->setAttack("sword");
			EntitySystem::GetComp<CombatComponent>(entity)->stats = CombatStats{ 100, 2, 0, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f };

			makePlayerGFX(entity);

			renderGroups[playerCamId].push_back(entity);

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

				renderGroups[playerCamId].push_back(id);

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
}

void Game::updateMainMenu() {
	auto& menu = menus.getMenu(mainMenu);
	menu.updateMenuEntries(menuCamId);

	std::ifstream settingsFile{ "settings.json" };
	json settings{};
	settingsFile >> settings;
	std::string address = settings["ip"];
	int port = settings["port"];
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
			std::string response{ r.text_box.resposne };
			if (r.entryTag == "ip_address") {
				settings["ip"] = response;
				address = response;
			}
			else if(r.entryTag == "port") {
				try {
					int newPort = std::stoi(response);
					settings["port"] = newPort;
					port = newPort;
				}
				catch (std::invalid_argument e) {}
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
	}
}

void Game::makePlayerGFX(EntityId playerId_) {
	EntitySystem::MakeComps<PlayerGC>(1, &playerId_);
	EntitySystem::GetComp<RenderComponent>(playerId_)->loadDrawable<AnimatedSprite>("character", Vec2i{ 64, 64 });
	EntitySystem::GetComp<PlayerGC>(playerId_)->loadAnimations();
	EntitySystem::GetComp<PlayerGC>(playerId_)->attackSprite = weapons.cloneAnimation("sword");
}

void Game::renderAll(double gfxDelay) {
	for (auto& pair : renderGroups) {
		GLRenderer::setCamera(pair.first);
		for (auto& entity : pair.second) {
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

GameState Game::getState() {
	return gameState;
}
