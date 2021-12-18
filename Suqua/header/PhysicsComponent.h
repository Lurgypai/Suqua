#pragma once
#include "AABB.h"
#include "PhysicsSystem.h"

class PhysicsComponent {
	friend class PhysicsSystem;

private:
	enum PhysicsData {
		WEIGHT = 4,			//float
		XVEL = 5,			//float
		YVEL = 6,			//float
		GROUNDED = 7,		//char
		FROZEN = 8,			//char
		WEIGHTLESS = 9,		//char
		COLLIDEABLE = 10,	//char
		XRES = 11,
		YRES = 12,
	};

public:

	PhysicsComponent(EntityId id_ = 0, AABB collider = AABB{ {0, 0}, {0, 0} }, float weight_ = 0, Vec2f vel = {0, 0}, bool collideable_ = false);
	EntityId getId() const;
	
	const AABB & getCollider() const;

	void refreshPos();

	bool intersects(const AABB & other);

	void move(Vec2f amount);
	void move(float angle, float amount);
	void accelerate(Vec2f amount);
	void accelerate(float angle, float amount);
	//move the bottom center to newPos
	void teleport(const Vec2f & newPos);
	Vec2f position() const;
	//move the pos to newPos
	void setPos(const Vec2f& newPos);
	Vec2f getPos() const;

	Vec2f center();

	Vec2f getRes() const;
	void setRes(const Vec2f & res_);

	Vec2f getVel() const;
	void setVel(const Vec2f& vel);

	void freeze();
	void unfreeze();
	void setFrozen(bool newFrozen);
	bool isFrozen() const;

	void setWeight(float newWeight);
	float getWeight() const;

	bool isGrounded() const;

	bool isCollideable() const;
	void setCollideable(bool newCollideable);

	bool isWeightless() const;
	void setWeigtless(bool newWeigtless);

private:
	void setGrounded(bool newGrounded);

protected:
	AABB collider;
	EntityId id;
};