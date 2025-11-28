#include "IABasicDash.h"
#include "PhysicsComponent.h"
#include "TopDownMoverComponent.h"

#include <print>

IABasicDash::IABasicDash() :
	dashTime{0.2f},
	cooldownTime{1.5f},
	elapsedTime{ 0.f },
	state{DashState::ready},
	dashDir{0.f, 0.f},
	dashSpeed{500.f},
	activatingEntity{0}
{}

void IABasicDash::update(float delta) {
	elapsedTime += delta;
	switch (state)
	{
	case IABasicDash::DashState::ready:
		break;
	case IABasicDash::DashState::dashing: {
		// continue dash
		auto* physicsComp = EntitySystem::GetComp<PhysicsComponent>(activatingEntity);
		if (physicsComp) {
			physicsComp->vel = dashDir;
			physicsComp->vel *= dashSpeed;
		}

		// finish dashing
		if (elapsedTime > dashTime) {
			state = DashState::cooldown;
			elapsedTime = 0;

			auto* moverComp = EntitySystem::GetComp<TopDownMoverComponent>(activatingEntity);
			if (moverComp) moverComp->paused = false;
		}
		break; }
	case IABasicDash::DashState::cooldown:
		if (elapsedTime > dashTime) {
			state = DashState::ready;
			elapsedTime = 0;
		}
		break;
	default:
		break;
	}
}

void IABasicDash::doAbility(Scene& scene, EntityId sourceEntity, const Controller& controller, InventoryItem& sourceInvItem) {
	activatingEntity = sourceEntity;
	if (state != DashState::ready) return;
	// begin dash
	dashDir = Vec2f{ 1.f, 0.f };
	dashDir.angle(controller.stick1.angle());
	auto* moverComp = EntitySystem::GetComp<TopDownMoverComponent>(sourceEntity);
	if (moverComp) moverComp->paused = true;

	// update state
	state = DashState::dashing;
	elapsedTime = 0;
}

std::unique_ptr<ItemAbility> IABasicDash::clone() const {
	return std::make_unique<IABasicDash>(*this);
}