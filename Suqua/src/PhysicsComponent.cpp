#include "PhysicsComponent.h"
#include "PositionComponent.h"
#include "AngleUtil.h"
#include "NetworkDataComponent.h"
#include "NetworkDataComponentDataFields.h"
#include <cmath>

using NDC = NetworkDataComponent;
using namespace PhysicsData;

PhysicsComponent::PhysicsComponent(EntityId id_,
        AABB collider_, float weight_, Vec2f vel_,
        bool collideable_, bool collideableWith_) :
	id{ id_ },
	collider{collider_},
	weight{nullptr},
	xVel{ nullptr },
	yVel{nullptr},
	grounded{nullptr},
	frozen{nullptr},
	weightless{nullptr},
	collides{nullptr},
	collidesWith{nullptr}
{
	if (id != 0) {
		if (!EntitySystem::Contains<PositionComponent>() || !EntitySystem::GetComp<PositionComponent>(id)) {
			EntitySystem::MakeComps<PositionComponent>(1, &id); }

		NDC* dataComp = EntitySystem::GetComp<NDC>(id);

		dataComp->set<float>(WEIGHT, weight_);
		weight = &dataComp->get<float>(WEIGHT);
		dataComp->set(XVEL, vel_.x);
		xVel = &dataComp->get<float>(XVEL);
		dataComp->set(YVEL, vel_.y);
		yVel = &dataComp->get<float>(YVEL);
		dataComp->set(GROUNDED, false);
		grounded = &dataComp->get<bool>(GROUNDED);
		dataComp->set(FROZEN, false);
		frozen = &dataComp->get<bool>(FROZEN);
		dataComp->set(WEIGHTLESS, false);
		weightless = &dataComp->get<bool>(WEIGHTLESS);

		dataComp->set(COLLIDEABLE, collideable_);
		collides = &dataComp->get<bool>(COLLIDEABLE);
        
		dataComp->set(COLLIDEABLE_WITH, collideableWith_);
		collidesWith = &dataComp->get<bool>(COLLIDEABLE_WITH);

		dataComp->set(XRES, collider.res.x);
		xRes = &dataComp->get<float>(XRES);
		dataComp->set(YRES, collider.res.y);
		yRes = &dataComp->get<float>(YRES);

		auto posComp = EntitySystem::GetComp<PositionComponent>(id);
		posComp->setPos(collider.pos);
	}
}

EntityId PhysicsComponent::getId() const {
	return id;
}

const AABB & PhysicsComponent::getCollider() const {
	return collider;
}

void PhysicsComponent::refreshPos() {
	auto posComp = EntitySystem::GetComp<PositionComponent>(id);
	collider.pos = posComp->getPos();
}

bool PhysicsComponent::intersects(const AABB & other) {
	return collider.intersects(other);
}

void PhysicsComponent::move(Vec2f amount) {
	collider.pos += amount;
	auto posComp = EntitySystem::GetComp<PositionComponent>(id);
	posComp->setPos(collider.pos);
}

void PhysicsComponent::move(float angle, float amount) {
	Vec2f displacement{1, 0};
	displacement.angle(angle);
	displacement *= amount;
	move(displacement);
}

void PhysicsComponent::accelerate(Vec2f amount) {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	*xVel += amount.x;
	*yVel += amount.y;
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
	posComp->setPos(collider.pos);
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
	posComp->setPos(collider.pos);
}

Vec2f PhysicsComponent::getRes() const {
	return collider.res;
}

void PhysicsComponent::setRes(const Vec2f& res_) {
	collider.res = res_;
	*xRes = collider.res.x;
	*yRes = collider.res.y;
}

Vec2f PhysicsComponent::getVel() const {
	return Vec2f{ *xVel, *yVel };
}

void PhysicsComponent::setVel(const Vec2f& newVel) {
	*xVel = newVel.x;
	*yVel = newVel.y;
}

void PhysicsComponent::freeze() {
	*frozen = true;
}

void PhysicsComponent::unfreeze() {
	*frozen = false;
}

bool PhysicsComponent::isFrozen() const {
	return *frozen;
}

void PhysicsComponent::setFrozen(bool newFrozen) {
	*frozen = newFrozen;
}

void PhysicsComponent::setWeight(float newWeight) {
	*weight = newWeight;
}

float PhysicsComponent::getWeight() const {
	return *weight;
}

bool PhysicsComponent::doesCollide() const {
	return *collides;
}

void PhysicsComponent::setDoesCollide(bool newCollideable) {
	*collides = newCollideable;
}

bool PhysicsComponent::isCollidedWith() const {
    return *collidesWith;
}

void PhysicsComponent::setCollidedWith(bool newCollideable) {
    *collidesWith = newCollideable;
}

bool PhysicsComponent::isWeightless() const {
	return *weightless;
}

void PhysicsComponent::setWeightless(bool newWeightless) {
	*weightless = newWeightless;
}

bool PhysicsComponent::isGrounded() const {
	return *grounded;
}

void PhysicsComponent::setGrounded(bool newGrounded) {
	*grounded = newGrounded;
}

void PhysicsComponent::onCollide(CollisionDir dir) {
    if(collisionHandler) {
        collisionHandler->onCollide(*this, dir);
    }
    else {
        switch(dir) {
            case CollisionDir::left:
            case CollisionDir::right:
                *xVel = 0;
                break;
            case CollisionDir::down:
                *grounded = true;
            case CollisionDir::up:
                *yVel = 0;
                break;
            default:
                break;
        }
    }
}
