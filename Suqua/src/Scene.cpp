#include "Scene.h"
#include <algorithm>
#include <iterator>
#include <iostream>
#include "Game.h"
#include "InputDevice.h"
#include "EntityBaseComponent.h"

std::vector<EntityId> Scene::addEntities(unsigned int count) {
	std::vector<EntityId> ids = std::vector<EntityId>(count, 0);
	EntitySystem::GenEntities(count, ids.data());
	for (auto&& id : ids) {
		entities.insert(id);
	}
	return std::move(ids);
}

void Scene::removeEntities(const std::vector<EntityId>& entities_) {
	EntitySystem::FreeEntities(entities_.size(), entities_.data());
	for (auto&& id : entities_) {
		entities.erase(entities.find(id));
	}
}

Scene::Scene(SceneId id_, FlagType flags_) : 
	id{id_},
	camId{0},
	entities{},
	flags{flags_}
{}

Scene::~Scene() {};

/*
* It might be good to add an optimization that doesn't store inputs if the networkInputDelay is 0.
*/

void Scene::storeInputs(Game& game) {
	for (auto&& pair : entityInputs) {
		ControllerComponent* cont = EntitySystem::GetComp<ControllerComponent>(pair.first);
		Controller c = game.getInputDevice(pair.second).getControllerState();
		
		// store future input
		futureEntityInputs[game.getGameTick() + game.networkInputDelay].emplace( pair.first, c );
	}
}

const std::unordered_map<EntityId, Controller>* Scene::getInputsAtTime(Tick time) const
{
	auto& pair = futureEntityInputs.find(time);
	if (pair != futureEntityInputs.end()) {
		return &pair->second;
	}
	return nullptr;
}

void Scene::applyInputs(Game& game) {
	for (auto&& pair : entityInputs) {
		ControllerComponent* cont = EntitySystem::GetComp<ControllerComponent>(pair.first);

		auto found = futureEntityInputs.find(game.getGameTick());
		if (found != futureEntityInputs.end()) {

			//found->second is the mapping of entity id -> controller
			//found->second.find(pair.first) finds the controller associated with the entity "pair.first"
			cont->setController(found->second.find(pair.first)->second);
			futureEntityInputs.erase(found);
		}
		// no input found
		// this can happen at the beginning, as the first inputs will be stored for the future.
	}
}

void Scene::removeAllEntities() {
	std::vector<EntityId> entityVec{ entities.begin(), entities.end() };
	EntitySystem::FreeEntities(entityVec.size(), entityVec.data());
}

void Scene::removeDeadEntities() {
	static auto isDead = [](EntityId id) {
		auto* base = EntitySystem::GetComp<EntityBaseComponent>(id);
		return base && base->isDead;
	};

	for (auto iter = entities.begin();;) {
		if (iter == entities.end()) break;
		if (isDead(*iter)) {
			iter = entities.erase(iter);
		}
		else ++iter;
	}
}

void Scene::drawScene(const RenderSystem& render) const {
	GLRenderer::setCamera(camId);
	if (EntitySystem::Contains<RenderComponent>()) {
		for (auto&& entity : entities) {
			RenderComponent* renderComp = EntitySystem::GetComp<RenderComponent>(entity);
			if (renderComp) {
				render.draw(*renderComp);
			}
		}
	}
}

const std::set<EntityId>& Scene::getEntities() const {
	return entities;
}

CamId Scene::getCamId() const {
	return camId;
}

SceneId Scene::getId() const {
	return id;
}

void Scene::addEntityInputs(const EntityInputSet& inputs) {
	for (auto&& pair : inputs) {
		entityInputs.emplace(pair.first, pair.second);
	}
}

void Scene::removeEntityInputs(EntityId id) {
	entityInputs.erase(id);
}
