
#include "Game.h"
#include <EntityBaseComponent.h>
#include <ControllerComponent.h>
#include <RectDrawable.h>
#include "TextDrawable.h"
#include "PositionComponent.h"
#include <fstream>

#include "nlohmann/json.hpp"
#include "../graphics/PlayerGC.h"
#include "../graphics/CapturePointGC.h"
#include "../player/ClientPlayerComponent.h"

using json = nlohmann::json;

Game::Game() :
	physics{},
	combat{},
	clientPlayers{&physics, &combat}
{}

void Game::startOfflineGame() {

	for (auto& entity : EntitySystem::GetPool<EntityBaseComponent>()) {
		entity.isDead = true;
	}

	editables.isEnabled = false;
	//load up the stage
	//grab the  player spawn point
	//create the player there
	
	std::ifstream settingsFile{ "settings.json" };
	json settings{};
	settingsFile >> settings;
	std::string stageName = settings["stage"];

	loadStage(stageName);

	playerId = players.makePlayer(weapons);

	EntitySystem::MakeComps<PlayerGC>(1, &playerId);
	EntitySystem::GetComp<RenderComponent>(playerId)->loadDrawable<AnimatedSprite>("images/stabbyman.png", Vec2i{ 64, 64 });
	EntitySystem::GetComp<PlayerGC>(playerId)->loadAnimations();
	EntitySystem::GetComp<PlayerGC>(playerId)->attackSprite = weapons.cloneAnimation("player_sword");

	EntitySystem::GetComp<PlayerLC>(playerId)->chooseSpawn();

	mode.load(&spawns, 2, 1, 6000);

	auto spawnables = stage.getSpawnables();
	EntitySystem::MakeComps<CapturePointGC>(spawnables.size(), spawnables.data());

	EntitySystem::GenEntities(1, &testText);
	EntitySystem::MakeComps<RenderComponent>(1, &testText);
	RenderComponent* testTextRender = EntitySystem::GetComp<RenderComponent>(testText);
	testTextRender->loadDrawable<TextDrawable>();
	testTextRender->getDrawable<TextDrawable>()->text = "Hello World!";
	testTextRender->getDrawable<TextDrawable>()->scale.x = 0.25;
	testTextRender->getDrawable<TextDrawable>()->scale.y = 0.25;
	testTextRender->getDrawable<TextDrawable>()->anti_alias = false;
	testTextRender->getDrawable<TextDrawable>()->font.loadFromFiles("suqua/fonts/consolas_0.png", "suqua/fonts/consolas.fnt");
	testTextRender->getDrawable<TextDrawable>()->setColor(1.0, 1.0, 1.0);
	EntitySystem::GetComp<PositionComponent>(testText)->pos = { 0, -30 };
}

void Game::startOnlineGame() {
	for (auto& entity : EntitySystem::GetPool<EntityBaseComponent>()) {
		entity.isDead = true;
	}
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

	EntitySystem::MakeComps<PlayerGC>(1, &playerId);
	EntitySystem::GetComp<RenderComponent>(playerId)->loadDrawable<AnimatedSprite>("images/stabbyman.png", Vec2i{ 64, 64 });
	EntitySystem::GetComp<PlayerGC>(playerId)->loadAnimations();
	EntitySystem::GetComp<PlayerGC>(playerId)->attackSprite = weapons.cloneAnimation("player_sword");

	//EntitySystem::GetComp<PlayerLC>(playerId)->chooseSpawn();

	mode.loadData(2, 1, 144000);

	/*
	auto spawnables = stage.getSpawnables();
	EntitySystem::MakeComps<CapturePointGC>(spawnables.size(), spawnables.data());
	*/

	client.setPlayer(playerId);
	client.setWeaponManager(weapons);
	client.setClientPlayerSystem(&clientPlayers);
	client.setOnlineSystem(&online);
	client.setMode(&mode);
	client.setSpawns(&spawns);

	client.connect(address, port);
}

void Game::startStageEditor(const std::string & filePath) {
	for (auto& entity : EntitySystem::GetPool<EntityBaseComponent>()) {
		entity.isDead = true;
	}

	editables.isEnabled = true;
	editables.load(filePath);
}

void Game::loadStage(const std::string& stageName) {
	stage = Stage(stageName, spawns);
	stage.loadGraphics();
}

EntityId Game::getPlayerId() {
	return playerId;
}

const Stage& Game::getStage() const {
	return stage;
}
