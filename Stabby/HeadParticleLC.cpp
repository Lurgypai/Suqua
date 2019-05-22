#include "pch.h"
#include "HeadParticleLC.h"
#include "ImageGC.h"

HeadParticleLC::HeadParticleLC(EntityId id_) :
	id{id_},
	gravity{3},
	life{0},
	pos{0, 0},
	vel{0, 0},
	collider{ pos, Vec2f{4, 4} }
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

void HeadParticleLC::update(double delta, const Stage& stage) {

	life--;
	if (life == 0) {
		EntitySystem::FreeComps<HeadParticleLC>(1, &id);
		EntitySystem::FreeComps<ImageGC>(1, &id);
	}
	else {
		vel.y += gravity;

		collider.setPos(pos);
		collider.setVel(vel);
		pos = collider.handleCollision(stage.getCollider(), delta);
		vel = collider.getVel();
	}
}
