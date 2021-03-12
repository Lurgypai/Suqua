#pragma once
#include "AABB.h"
#include "PhysicsSystem.h"
#include "PhysicsData.h"

class PhysicsComponent {
	friend class PhysicsSystem;

public:

	PhysicsComponent(EntityId id_ = 0, AABB collider = AABB{ {0, 0}, {0, 0} }, float weight = 0, Vec2f vel = {0, 0}, bool collideable_ = false);
	EntityId getId() const;
	
	const AABB & getCollider() const;

	void reevaluatePosition();

	bool intersects(const AABB & other);

	void move(Vec2f amount);
	void move(float angle, float amount);
	void accelerate(Vec2f amount);
	void accelerate(float angle, float amount);
	void teleport(const Vec2f & newPos);
	Vec2f getPos() const;

	Vec2f center();

	Vec2f getRes() const;
	void setRes(Vec2f res_);

protected:
	AABB collider;
	EntityId id;
};