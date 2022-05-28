#include "ZombieComponent.h"
#include "PhysicsComponent.h"


using NDC = NetworkDataComponent;
using namespace ZombieData;

ZombieComponent::ZombieComponent(EntityId id_) :
	id{ id_ },
	groundedDecel{ 6 },
	idleTime{ 15 },
	chargeTime{ 60 },
	moveSpeed{ 300 },
	launchSpeed{ 200, -100 }
{
	if (id != 0) {
		if (!EntitySystem::Contains<PhysicsComponent>() || !EntitySystem::GetComp<PhysicsComponent>(id)) {
			EntitySystem::MakeComps<PhysicsComponent>(1, &id);
			auto* physics = EntitySystem::GetComp<PhysicsComponent>(id);
			physics->setRes({ 6, 17 });
			physics->teleport({ 480 / 2, 270 / 2 });
			physics->setWeight(4);
		}

		if (!EntitySystem::Contains<ControllerComponent>() || !EntitySystem::GetComp<ControllerComponent>(id)) {
			EntitySystem::MakeComps<ControllerComponent>(1, &id);
		}

		NDC* data = EntitySystem::GetComp<NDC>(id);
		data->set<uint32_t>(ACTION_FRAME, 0);
		data->set<int32_t>(DIR, 1);
		data->set<uint32_t>(STATE, ZombieState::idle);
		data->set<uint32_t>(HEALTH, 100);
	}
}

EntityId ZombieComponent::getId() const {
	return id;
}

void ZombieComponent::update(Game& game) {
	NDC* data = EntitySystem::GetComp<NDC>(id);

	switch (data->get<uint32_t>(STATE)) {
	case ZombieState::idle: idle_(); break;
	case ZombieState::moving: moving_(); break;
	case ZombieState::charging: charging_(); break;
	case ZombieState::attacking: attacking_(); break;
	case ZombieState::hurt: hurt_(); break;
	}


}

ZombieComponent::ZombieState ZombieComponent::getState() const {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	return static_cast<ZombieState>(data->get<uint32_t>(STATE));
}

void ZombieComponent::damage(int damage) {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	auto& health = data->get<uint32_t>(HEALTH);
	if (health < damage) health = 0;
	else health -= damage;
}

void ZombieComponent::markHit(EntityId target) {
	hitEntities.emplace(target);
}

bool ZombieComponent::hasHit(EntityId target) {
	return hitEntities.find(target) != hitEntities.end();
}

bool ZombieComponent::hitboxChanged() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	return data->get<uint32_t>(STATE) != ZombieState::attacking;
}

void ZombieComponent::clearMarked() {
	hitEntities.clear();
}

int ZombieComponent::getDir() const
{
	NDC* data = EntitySystem::GetComp<NDC>(id);
	return data->get<int32_t>(DIR);
}

void ZombieComponent::beginIdle() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	data->get<uint32_t>(STATE) = ZombieState::idle;

	data->get<uint32_t>(ACTION_FRAME) = idleTime;
}

void ZombieComponent::idle_() {
	NDC* data = EntitySystem::GetComp<NDC>(id);

	auto& actionFrame = data->get<uint32_t>(ACTION_FRAME);
	if (actionFrame > 0) {
		--actionFrame;
		return;
	}


	ControllerComponent* cont = EntitySystem::GetComp<ControllerComponent>(id);
	auto& c = cont->getController();

	if (c[ControllerBits::BUTTON_11]) {
		beginCharging();
		return;
	}
	if (c[ControllerBits::LEFT] || c[ControllerBits::RIGHT]) {
		beginMoving();
		return;
	}
}

void ZombieComponent::beginMoving() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	data->get<uint32_t>(STATE) = ZombieState::moving;

	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);
	ControllerComponent* cont = EntitySystem::GetComp<ControllerComponent>(id);
	const Controller& c = cont->getController();

	int inputDir = 0;
	if (c[ControllerBits::LEFT]) --inputDir;
	if (c[ControllerBits::RIGHT]) ++inputDir;

	if (inputDir != 0) data->get<int32_t>(DIR) = inputDir;

	physics->accelerate(Vec2f{ static_cast<float>(inputDir) * moveSpeed, 0 });

}

void ZombieComponent::moving_() {
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);

	if (physics->getVel().x > groundedDecel / 2) {
		physics->accelerate({ -groundedDecel, 0 });
	}
	else if (physics->getVel().x < -groundedDecel / 2) {
		physics->accelerate({ groundedDecel, 0 });
	}
	else {
		physics->setVel({ 0, 0 });
		beginIdle();
	}
}

void ZombieComponent::beginCharging() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	data->get<uint32_t>(STATE) = ZombieState::charging;

	data->get<uint32_t>(ACTION_FRAME) = chargeTime;
}

void ZombieComponent::charging_() {
	NDC* data = EntitySystem::GetComp<NDC>(id);

	auto& actionFrame = data->get<uint32_t>(ACTION_FRAME);
	if (actionFrame > 0) {
		--actionFrame;
		return;
	}

	beginAttacking();
}

void ZombieComponent::beginAttacking() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	data->get<uint32_t>(STATE) = ZombieState::attacking;

	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);

	physics->accelerate(Vec2f{ static_cast<float>(data->get<int32_t>(DIR)) * launchSpeed.x, launchSpeed.y });
}

void ZombieComponent::attacking_() {
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);

	if (physics->isGrounded()) {
		if (physics->getVel().x > groundedDecel / 2) {
			physics->accelerate({ -groundedDecel, 0 });
		}
		else if (physics->getVel().x < -groundedDecel / 2) {
			physics->accelerate({ groundedDecel, 0 });
		}
		else {
			physics->setVel({ 0, 0 });
			beginIdle();
		}
	}
}

void ZombieComponent::beginHurt()
{
}

void ZombieComponent::hurt_()
{
}


