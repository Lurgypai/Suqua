#pragma once
#include <memory>

#include "ComponentMacros.h"
#include "AABB.h"
#include "CollisionHandler.h"

class PhysicsComponent {
	friend class PhysicsSystem;
    CompMembers(PhysicsComponent);
public:
	PhysicsComponent(EntityId id_,
            const Vec2f& pos,
            const Vec2f& res,
            bool collideable_,
            bool collideableWith_,
            float weight_ = 0.f,
            bool weightless_ = true);
	
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
