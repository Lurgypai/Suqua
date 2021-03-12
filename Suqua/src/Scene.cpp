#include "Scene.h"
#include <algorithm>
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

Scene::Scene(SceneId id_) : 
	id{id_},
	entities{}
{}

void Scene::unload() {
	std::vector<EntityId> entityVec{entities.begin(), entities.end()};
	EntitySystem::FreeEntities(entityVec.size(), entityVec.data());
}

void Scene::removeDeadEntites() {
	for (auto&& base : EntitySystem::GetPool<EntityBaseComponent>()) {
		if (base.isDead)
			entities.erase(entities.find(base.getId()));
	}
}

void Scene::drawScene(RenderSystem& render) const {
	GLRenderer::setCamera(camId);
	for (auto&& entity : entities) {
		RenderComponent* renderComp = EntitySystem::GetComp<RenderComponent>(entity);
		if (renderComp) {
			render.draw(*renderComp);
		}
	}
}

const std::set<EntityId>& Scene::getEntities() const {
	return entities;
}

CamId Scene::getCamId() const {
	return camId;
}
