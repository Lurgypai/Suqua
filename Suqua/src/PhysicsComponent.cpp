#include "PhysicsComponent.h"
#include "AngleUtil.h"
#include "PositionComponent.h"
#include <cmath>

PhysicsComponent::PhysicsComponent(EntityId id_, AABB collider_, float weight_, Vec2f vel_, bool collideable_) :
	id{ id_ },
	collider{collider_},
	weight{weight_},
	vel{vel_},
	grounded{false},
	frozen{false},
	weightless{false},
	collideable{collideable_}
{
	if (id != 0) {
		if (!EntitySystem::Contains<PositionComponent>() || EntitySystem::GetComp<PositionComponent>(id) == nullptr) {
			EntitySystem::MakeComps<PositionComponent>(1, &id);
			PositionComponent * posComp = EntitySystem::GetComp<PositionComponent>(id);
			posComp->pos = collider.pos;
		}
		else {
			PositionComponent * posComp = EntitySystem::GetComp<PositionComponent>(id);
			collider.pos = posComp->pos;
		}
	}
}

EntityId PhysicsComponent::getId() const {
	return id;
}

const AABB & PhysicsComponent::getCollider() const {
	return collider;
}

void PhysicsComponent::reevaluatePosition() {
	collider.pos = EntitySystem::GetComp<PositionComponent>(id)->pos;
}

bool PhysicsComponent::intersects(const AABB & other) {
	return collider.intersects(other);
}

void PhysicsComponent::move(Vec2f amount) {
	PositionComponent * posComp = EntitySystem::GetComp<PositionComponent>(id);
	posComp->pos += amount;
	collider.pos = posComp->pos;
}

void PhysicsComponent::move(float angle, float amount) {
	Vec2f displacement{0, 0};
	displacement.x = std::cos(radians(angle)) * amount;
	displacement.y = std::sin(radians(angle)) * amount;
	collider.pos += displacement;
}

void PhysicsComponent::accelerate(Vec2f amount) {
	vel += amount;
}

void PhysicsComponent::accelerate(float angle, float amount) {
	Vec2f acceleration{ 0, 0 };
	acceleration.x = std::cos(radians(angle)) * amount;
	acceleration.y = std::sin(radians(angle)) * amount;
	vel += acceleration;
}

void PhysicsComponent::teleport(const Vec2f & newPos) {
	PositionComponent * position = EntitySystem::GetComp<PositionComponent>(id);
	position->pos = newPos - Vec2f{collider.res.x / 2, collider.res.y};
	collider.pos = position->pos;
}

Vec2f PhysicsComponent::getPos() const {
	return collider.pos + Vec2f{ collider.res.x / 2, collider.res.y };
}

Vec2f PhysicsComponent::center() {
	return collider.center();
}

Vec2f PhysicsComponent::getRes() const {
	return collider.res;
}

void PhysicsComponent::setRes(Vec2f res_) {
	collider.res = res_;
}
