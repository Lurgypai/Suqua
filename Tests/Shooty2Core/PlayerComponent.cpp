#include "PlayerComponent.h"
#include "RenderComponent.h"
#include "PhysicsComponent.h"
#include "ControllerComponent.h"
#include "TopDownMoverComponent.h"
#include "TeamComponent.h"

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
		if (!EntitySystem::Contains<TopDownMoverComponent>() || !EntitySystem::GetComp<TopDownMoverComponent>(id)) {
			EntitySystem::MakeComps<TopDownMoverComponent>(1, &id);
		} 
		//if (!EntitySystem::Contains<TeamComponent>() || !EntitySystem::GetComp<TeamComponent>(id)) {
		//	EntitySystem::MakeComps<TeamComponent>(1, &id);
		//}

		auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
		physicsComp->setRes({ 6, 4 });
		physicsComp->teleport({ 0, 0 });
		physicsComp->setCollideable(true);

		auto ndc = EntitySystem::GetComp<NDC>(id);
		ndc->set<std::uint32_t>(playerStateVarId, static_cast<uint32_t>(State::idle));
		ndc->set<float>(playerDirVarId, 0.0f);

		auto topDownMoverComp = EntitySystem::GetComp<TopDownMoverComponent>(id);
		topDownMoverComp->setMoveSpeed(50.0);
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
	}
}

void PlayerComponent::beginIdle() {
	setPlayerState(State::idle);
}

void PlayerComponent::idle_() {
	auto& controller = getController();
}
