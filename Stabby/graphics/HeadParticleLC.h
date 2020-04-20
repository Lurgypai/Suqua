#pragma once
#include "EntitySystem.h"
#include "Stage.h"
#include "PhysicsComponent.h"

class HeadParticleLC {
public:
	HeadParticleLC( EntityId id_ = 0 );

	void setLife(unsigned int life_);
	void update(double delta);
	Vec2f getPos();
	EntityId getId() const;
private:
	EntityId id;
	unsigned int life;
};