#pragma once
#include "AABB.h"
#include "PhysicsSystem.h"
#include "CollisionHandler.h"
#include <memory>

class PhysicsComponent {
	friend class PhysicsSystem;
public:

	PhysicsComponent(EntityId id_ = 0,
            AABB collider = AABB{ {0, 0}, {0, 0} },
            float weight_ = 0, Vec2f vel = {0, 0},
            bool collideable_ = true, bool collideableWith_ = true);
	EntityId getId() const;
	
	const AABB & getCollider() const;

	void refreshPos();
    template <typename Handler, typename... Args>
    void loadCollisionHandler(Args... args);

	bool intersects(const AABB & other);

	void move(Vec2f amount);
	void move(float angle, float amount);
	void accelerate(Vec2f amount);
	void accelerate(float angle, float amount);
	//move the bottom center to newPos
	void teleport(const Vec2f & newPos);
	Vec2f position() const;

	Vec2f center();
	void center(const Vec2f& center_);

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

	bool doesCollide() const;
	void setDoesCollide(bool newCollideable);

    bool isCollidedWith() const;
    void setCollidedWith(bool newCollideable);

	bool isWeightless() const;
	void setWeightless(bool newWeigtless);
    
    void onCollide(CollisionDir dir);

private:

private:
	void setGrounded(bool newGrounded);

protected:
	EntityId id;

	AABB collider;
	float* weight;
	float* xVel;
	float* yVel;
	bool* grounded;
	bool* frozen;
	bool* weightless;
	bool* collides;
	bool* collidesWith;
	float* xRes;
	float* yRes;

    std::unique_ptr<CollisionHandler> collisionHandler;
};

template<typename Handler, typename... Args>
void PhysicsComponent::loadCollisionHandler(Args... args) {
    collisionHandler = std::make_unique<Handler>(args...);
}
