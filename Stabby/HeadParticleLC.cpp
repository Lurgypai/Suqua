#include "pch.h"
#include "HeadParticleLC.h"
#include "ImageGC.h"

HeadParticleLC::HeadParticleLC(EntityId id_) :
	id{id_},
	gravity{3},
	life{0},
	pos{0, 0},
	vel{0, 0}
{}

Vec2f HeadParticleLC::getPos() const {
	return pos;
}

void HeadParticleLC::setLife(unsigned int life_) {
	life = life_;
}

void HeadParticleLC::setPos(Vec2f pos_) {
	pos = pos_;
}

void HeadParticleLC::setVel(Vec2f vel_) {
	vel = vel_;
}

EntityId HeadParticleLC::getId() const {
	return id;
}

void HeadParticleLC::update(double delta) {

	PhysicsAABBLC * collider = EntitySystem::GetComp<PhysicsAABBLC>(id);

	life--;
	if (life == 0) {
		EntitySystem::FreeComps<HeadParticleLC>(1, &id);
		EntitySystem::FreeComps<PhysicsAABBLC>(1, &id);
		EntitySystem::FreeComps<ImageGC>(1, &id);
	}
	else if (collider != nullptr) {
		vel.y += gravity;

		collider->setPos(pos);
		collider->setVel(vel);
		pos = collider->handleCollisions(*EntitySystem::GetPool<AABBLC>(), delta);
		vel = collider->getVel();
	}
}
