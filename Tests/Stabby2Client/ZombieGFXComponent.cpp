#include "ZombieGFXComponent.h"
#include "RenderComponent.h"
#include "AnimatedSprite.h"
#include "PhysicsComponent.h"
#include "../Stabby2Core/ZombieComponent.h"

ZombieGFXComponent::ZombieGFXComponent(EntityId id_) :
	id{ id_ }
{
	if (id != 0) {
		if (!EntitySystem::Contains<RenderComponent>() || !EntitySystem::GetComp<RenderComponent>(id)) {
			EntitySystem::MakeComps<RenderComponent>(1, &id);
			auto* render = EntitySystem::GetComp<RenderComponent>(id);
			render->loadDrawable<AnimatedSprite>("zombie", "zombie.json");
			render->offset = { 13, 15 };


			auto* sprite = render->getDrawable<AnimatedSprite>();
			sprite->looping = false;
			sprite->setAnimation("Idle");
		}
	}
}

EntityId ZombieGFXComponent::getId() const {
	return id;
}

void ZombieGFXComponent::update(int millis) {
	auto* render = EntitySystem::GetComp<RenderComponent>(id);
	auto* sprite = render->getDrawable<AnimatedSprite>();

	auto* zombie = EntitySystem::GetComp<ZombieComponent>(id);
	auto* physics = EntitySystem::GetComp<PhysicsComponent>(id);

	if(zombie->getState() != ZombieComponent::ZombieState::freeze) sprite->update(millis);

	if (prevState != zombie->getState()) {
		switch (zombie->getState()) {
		case ZombieComponent::ZombieState::idle:
			sprite->looping = false;
			sprite->setAnimation("Idle");
			break;
		case ZombieComponent::ZombieState::moving:
			sprite->looping = false;
			sprite->setAnimation("Move");
			break;
		case ZombieComponent::ZombieState::hurt:
			sprite->looping = false;
			sprite->setAnimation("Hurt");
			break;
		case ZombieComponent::ZombieState::charging:
			sprite->looping = false;
			sprite->setAnimation("Attack");
			break;
		case ZombieComponent::ZombieState::attacking:
			sprite->looping = false;
			sprite->setAnimation("Attack");
			break;
		}
	}
	if (zombie->getDir() == 1) {
		sprite->setHorizontalFlip(false);
	}
	else {

		sprite->setHorizontalFlip(true);
	}

	prevState = zombie->getState();
}
