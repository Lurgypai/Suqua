#include "Scene.h"
#include <algorithm>
#include <iterator>
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

void Scene::applyInputs(Game& game) {
	for (auto&& pair : entityInputs) {
		game.getInputDevice(pair.second).doInput(pair.first);
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
	std::copy(inputs.begin(), inputs.end(), std::inserter(entityInputs, entityInputs.end()));
}

void Scene::removeEntityInputs(const EntityInputSet& inputs) {
	for (auto&& input : inputs) {
		entityInputs.erase(input);
	}
}
