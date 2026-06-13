#include "PhysicsComponent.h"
#include "PositionComponent.h"
#include "AngleUtil.h"
#include <cmath>

PhysicsComponent::PhysicsComponent(EntityId id_,
        const Vec2f& pos,
        const Vec2f& res,
        bool collideable_,
        bool collideableWith_,
        float weight_,
        bool weightless_) :
	id{ id_ },
	collider{ {}, res},
	weight{weight_},
    vel{0.f, 0.f},
	grounded{false},
	frozen{false},
	weightless{weightless_},
	collides{collideable_},
	collidesWith{collideableWith_}
{
    if (!EntitySystem::Contains<PositionComponent>() || !EntitySystem::GetComp<PositionComponent>(id)) {
        EntitySystem::MakeComps<PositionComponent>(1, &id); }
    teleport(pos);
}

const AABB & PhysicsComponent::getCollider() const {
	return collider;
}

void PhysicsComponent::refreshPos() {
	auto posComp = EntitySystem::GetComp<PositionComponent>(id);
	collider.pos = posComp->pos;
}

bool PhysicsComponent::intersects(const AABB & other) {
	return collider.intersects(other);
}

void PhysicsComponent::move(Vec2f amount) {
	collider.pos += amount;
	auto posComp = EntitySystem::GetComp<PositionComponent>(id);
    posComp->pos = collider.pos;
}

void PhysicsComponent::move(float angle, float amount) {
	Vec2f displacement{1, 0};
	displacement.angle(angle);
	displacement *= amount;
	move(displacement);
}

void PhysicsComponent::accelerate(Vec2f amount) {
    vel += amount;
}

void PhysicsComponent::accelerate(float angle, float amount) {
	Vec2f acceleration{ 0, 0 };
	acceleration.x = std::cos(radians(angle)) * amount;
	acceleration.y = std::sin(radians(angle)) * amount;

	accelerate(acceleration);
}

void PhysicsComponent::teleport(const Vec2f & newPos) {
	collider.pos = newPos - Vec2f{collider.res.x / 2, collider.res.y};
	auto posComp = EntitySystem::GetComp<PositionComponent>(id);
    posComp->pos = collider.pos;
}

Vec2f PhysicsComponent::position() const {
	return collider.pos + Vec2f{ collider.res.x / 2, collider.res.y };
}

Vec2f PhysicsComponent::center() {
	return collider.center();
}

void PhysicsComponent::center(const Vec2f& center_) {
	collider.center(center_);
	auto posComp = EntitySystem::GetComp<PositionComponent>(id);
    posComp->pos = collider.pos;
}

Vec2f PhysicsComponent::getRes() const {
	return collider.res;
}

void PhysicsComponent::setRes(const Vec2f& res_) {
	collider.res = res_;
}

void PhysicsComponent::freeze() {
	frozen = true;
}

void PhysicsComponent::unfreeze() {
	frozen = false;
}

bool PhysicsComponent::isFrozen() const {
	return frozen;
}

void PhysicsComponent::setFrozen(bool newFrozen) {
	frozen = newFrozen;
}

void PhysicsComponent::setWeight(float newWeight) {
	weight = newWeight;
}

float PhysicsComponent::getWeight() const {
	return weight;
}

bool PhysicsComponent::doesCollide() const {
	return collides;
}

void PhysicsComponent::setDoesCollide(bool newCollideable) {
	collides = newCollideable;
}

bool PhysicsComponent::isCollidedWith() const {
    return collidesWith;
}

void PhysicsComponent::setCollidedWith(bool newCollideable) {
    collidesWith = newCollideable;
}

bool PhysicsComponent::isWeightless() const {
	return weightless;
}

void PhysicsComponent::setWeightless(bool newWeightless) {
	weightless = newWeightless;
}

bool PhysicsComponent::isGrounded() const {
	return grounded;
}

void PhysicsComponent::setGrounded(bool newGrounded) {
	grounded = newGrounded;
}

void PhysicsComponent::onCollide(CollisionDir dir) {
    if(collisionHandler) {
        collisionHandler->onCollide(*this, dir);
    }
    else {
        switch(dir) {
            case CollisionDir::left:
            case CollisionDir::right:
                vel.x = 0;
                break;
            case CollisionDir::down:
                grounded = true;
            case CollisionDir::up:
                vel.y = 0;
                break;
            default:
                break;
        }
    }
}
