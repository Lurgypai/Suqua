#pragma once
#include "EntitySystem.h"
#include "PhysicsAABBLC.h"

class HeadParticleLC {
public:
	HeadParticleLC( EntityId id_ = 0 );

	Vec2f getPos() const;

	void setLife(unsigned int life_);
	void setPos(Vec2f pos);
	void setVel(Vec2f vel);
	EntityId getId() const;
	void update(double delta);
private:
	float gravity;
	unsigned int life;
	Vec2f vel;
	Vec2f pos;
	EntityId id;
};