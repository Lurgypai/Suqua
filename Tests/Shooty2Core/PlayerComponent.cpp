#include "PlayerComponent.h"
#include "RenderComponent.h"
#include "PhysicsComponent.h"
#include "ControllerComponent.h"

using NDC = NetworkDataComponent;

PlayerComponent::PlayerComponent(EntityId id_) : id { id_ }
{
	if (id != 0) {
		if (!EntitySystem::Contains<PhysicsComponent>() || !EntitySystem::GetComp<PhysicsComponent>(id)) {
			EntitySystem::MakeComps<PhysicsComponent>(1, &id);
		}
		if (!EntitySystem::Contains<ControllerComponent>() || !EntitySystem::GetComp<ControllerComponent>(id)) {
			EntitySystem::MakeComps<ControllerComponent>(1, &id);
		}

		auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
		physicsComp->setRes({ 6, 4 });
		physicsComp->teleport({ 20, 30 });

		auto ndc = EntitySystem::GetComp<NDC>(id);
		ndc->set<std::uint32_t>(playerStateVarId, static_cast<uint32_t>(State::idle));
	}
}

EntityId PlayerComponent::getId() const {
	return id;
}

void PlayerComponent::update()
{
	auto currState = getPlayerState();
	switch (currState) {
	case State::idle: idle_(); break;
	case State::walking: walking_(); break;
	}
}

void PlayerComponent::beginIdle() {
	setPlayerState(State::idle);
}

void PlayerComponent::idle_() {
	auto& controller = getController();

	int horizDir = 0, vertDir = 0;
	if (controller[ControllerBits::LEFT]) --horizDir;
	if (controller[ControllerBits::RIGHT]) ++horizDir;
	if (controller[ControllerBits::UP]) --vertDir;
	if (controller[ControllerBits::DOWN]) ++vertDir;

	if (horizDir != 0 || vertDir != 0) {
		beginWalking();
		return;
	}

	auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
	physicsComp->setVel({ 0, 0 });
}

void PlayerComponent::beginWalking() {
	setPlayerState(State::walking);
}

void PlayerComponent::walking_() {
	auto& controller = getController();
	int horizDir = 0, vertDir = 0;
	if (controller[ControllerBits::LEFT]) --horizDir;
	if (controller[ControllerBits::RIGHT]) ++horizDir;
	if (controller[ControllerBits::UP]) --vertDir;
	if (controller[ControllerBits::DOWN]) ++vertDir;

	if (vertDir == 0 && horizDir == 0) {
		beginIdle();
		return;
	}

	auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
	physicsComp->setVel({ horizDir * 20.0f, vertDir * 20.0f });
}
