
#include "HeadParticleLC.h"
#include "RenderComponent.h"

HeadParticleLC::HeadParticleLC(EntityId id_) :
	id{id_},
	life{0}
{}

void HeadParticleLC::setLife(unsigned int life_) {
	life = life_;
}

void HeadParticleLC::update(double delta) {
	life--;
	if (life == 0) {
		EntitySystem::FreeComps<HeadParticleLC>(1, &id);
		EntitySystem::FreeComps<PhysicsComponent>(1, &id);
		EntitySystem::FreeComps<RenderComponent>(1, &id);
	}
}

EntityId HeadParticleLC::getId() const {
	return id;
}
