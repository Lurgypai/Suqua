#include "Scene.h"
#include <iostream>
#include "Game.h"
#include "InputDevice.h"
#include "EntityBaseComponent.h"
#include "Packet.h"
#include "ControllerComponent.h"
#include "NetworkDataComponent.h"

// #include "DebugFIO.h"

std::vector<EntityId> Scene::addEntities(unsigned int count) {
	std::vector<EntityId> ids = std::vector<EntityId>(count, 0);
	EntitySystem::GenEntities(count, ids.data());
	for (auto&& id : ids) {
		entities.insert(id);
	}
	return ids;
}

void Scene::removeEntities(const std::vector<EntityId>& entities_) {
	EntitySystem::FreeEntities(entities_.size(), entities_.data());
	for (auto&& id : entities_) {
		entities.erase(entities.find(id));
	}
}

void Scene::broadcastDeadEntities(Game& game) {
    ByteStream deadEntityPacket;
    deadEntityPacket << Packet::DeadEntities;

    for(auto& entity : entities) {
        EntityBaseComponent* base = EntitySystem::GetComp<EntityBaseComponent>(entity);
        NetworkDataComponent* ndc = EntitySystem::GetComp<NetworkDataComponent>(entity);
        if(!base || !base->isDead ||
                !ndc || ndc->owner != NetworkDataComponent::Owner::local_shared) continue;

        auto& uuid = ndc->getUUID();
        game.networkEntityOwnershipSystem.removeLocalEntity(uuid);
        deadEntityPacket << uuid;

        // DebugFIO::TimeOut("send.packet.log") << online->getId() << " dead\n";
    }

    game.host.bufferAllDataByChannel(0, deadEntityPacket);
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
void Scene::doInputs(Game& game) {
    for (auto& controllerComp : EntitySystem::GetPool<ControllerComponent>()) {
        auto entityId = controllerComp.getId();
        auto iter = entityInputs.find(entityId);
        if(iter != entityInputs.end()) {
            Controller c = game.getInputDevice(iter->second).getControllerState();
            controllerComp.setController(c);
        }
        else {
            controllerComp.getController().storePrev();
        }
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
            if(EntitySystem::Contains<NetworkDataComponent>()) {
                auto ndc = EntitySystem::GetComp<NetworkDataComponent>(*iter);
                NetworkDataComponent::RemoveEntity(ndc->getUUID());
            }

			iter = entities.erase(iter);
		}
		else ++iter;
	}
}

void Scene::drawScene(const RenderSystem& render) const {
	GLRenderer::setCamera(camId);
	if (EntitySystem::Contains<RenderComponent>()) {
		for (auto&& entity : entities) {
			auto baseComp = EntitySystem::GetComp<EntityBaseComponent>(entity);
			if (!baseComp->isActive) continue;

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
