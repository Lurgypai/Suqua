#include "PhysicsComponent.h"
#include "AngleUtil.h"
#include "NetworkDataComponent.h"
#include "PositionData.h"
#include <cmath>

using NDC = NetworkDataComponent;

PhysicsComponent::PhysicsComponent(EntityId id_, AABB collider_, float weight_, Vec2f vel_, bool collideable_) :
	id{ id_ },
	collider{collider_}
{
	if (id != 0) {
		if (!EntitySystem::Contains<NDC>() || EntitySystem::GetComp<NDC>(id) == nullptr) {
			EntitySystem::MakeComps<NDC>(1, &id);
			NDC* dataComp = EntitySystem::GetComp<NDC>(id);

			dataComp->set(X, collider.pos.x);
			dataComp->set(Y, collider.pos.y);
		}
		else {
			NDC * dataComp = EntitySystem::GetComp<NDC>(id);

			collider.pos = { dataComp->get<float>(X), dataComp->get<float>(Y) };
		}

		NDC* dataComp = EntitySystem::GetComp<NDC>(id);

		dataComp->set<float>(WEIGHT, weight_);
		dataComp->set(XVEL, vel_.x);
		dataComp->set(YVEL, vel_.y);
		dataComp->set(GROUNDED, false);
		dataComp->set(FROZEN, false);
		dataComp->set(WEIGHTLESS, false);
		dataComp->set(COLLIDEABLE, collideable_);
		dataComp->set(XRES, collider.res.x);
		dataComp->set(YRES, collider.res.y);
	}
}

EntityId PhysicsComponent::getId() const {
	return id;
}

const AABB & PhysicsComponent::getCollider() const {
	return collider;
}

void PhysicsComponent::refreshPos() {
	NDC* dataComp = EntitySystem::GetComp<NDC>(id);
	collider.pos = { dataComp->get<float>(X), dataComp->get<float>(Y) };
}

bool PhysicsComponent::intersects(const AABB & other) {
	return collider.intersects(other);
}

void PhysicsComponent::move(Vec2f amount) {
	collider.pos += amount;
	NDC* dataComp = EntitySystem::GetComp<NDC>(id);
	dataComp->set(X, collider.pos.x);
	dataComp->set(Y, collider.pos.y);
}

void PhysicsComponent::move(float angle, float amount) {
	Vec2f displacement{0, 0};
	displacement.x = std::cos(radians(angle)) * amount;
	displacement.y = std::sin(radians(angle)) * amount;
	collider.pos += displacement;
}

void PhysicsComponent::accelerate(Vec2f amount) {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	data->get<float>(XVEL) += amount.x;
	data->get<float>(YVEL) += amount.y;
}

void PhysicsComponent::accelerate(float angle, float amount) {
	Vec2f acceleration{ 0, 0 };
	acceleration.x = std::cos(radians(angle)) * amount;
	acceleration.y = std::sin(radians(angle)) * amount;

	NDC* data = EntitySystem::GetComp<NDC>(id);
	data->get<float>(XVEL) += acceleration.x;
	data->get<float>(YVEL) += acceleration.y;
}

void PhysicsComponent::teleport(const Vec2f & newPos) {
	collider.pos = newPos - Vec2f{collider.res.x / 2, collider.res.y};

	NDC* data = EntitySystem::GetComp<NDC>(id);
	data->set(X, collider.pos.x);
	data->set(Y, collider.pos.y);
}

Vec2f PhysicsComponent::position() const {
	return collider.pos + Vec2f{ collider.res.x / 2, collider.res.y };
}

void PhysicsComponent::setPos(const Vec2f& newPos) {
	collider.pos = newPos;

	NDC* data = EntitySystem::GetComp<NDC>(id);
	data->set(X, collider.pos.x);
	data->set(Y, collider.pos.y);
}

Vec2f PhysicsComponent::getPos() const {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	return { data->get<float>(X), data->get<float>(Y) };
}

Vec2f PhysicsComponent::center() {
	return collider.center();
}

Vec2f PhysicsComponent::getRes() const {
	return collider.res;
}

void PhysicsComponent::setRes(const Vec2f& res_) {
	collider.res = res_;

	NDC* data = EntitySystem::GetComp<NDC>(id);
	data->set(XRES, collider.res.x);
	data->set(YRES, collider.res.y);
}

Vec2f PhysicsComponent::getVel() const {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	return Vec2f{ data->get<float>(XVEL), data->get<float>(YVEL) };
}

void PhysicsComponent::setVel(const Vec2f& newVel) {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	data->set(XVEL, newVel.x);
	data->set(YVEL, newVel.y);
}

void PhysicsComponent::freeze() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	data->set(FROZEN, true);
}

void PhysicsComponent::unfreeze() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	data->set(FROZEN, false);
}

bool PhysicsComponent::isFrozen() const {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	return data->get<bool>(FROZEN);
}

void PhysicsComponent::setFrozen(bool newFrozen) {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	data->set(FROZEN, newFrozen);
}

void PhysicsComponent::setWeight(float newWeight) {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	data->set(WEIGHT, newWeight);
}

float PhysicsComponent::getWeight() const {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	return data->get<float>(WEIGHT);
}

bool PhysicsComponent::isCollideable() const {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	return data->get<bool>(COLLIDEABLE);
}

void PhysicsComponent::setCollideable(bool newCollideable) {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	data->set(COLLIDEABLE, newCollideable);
}

bool PhysicsComponent::isWeightless() const {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	return data->get<bool>(WEIGHTLESS);
}

void PhysicsComponent::setWeigtless(bool newWeigtless) {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	data->set(WEIGHTLESS, newWeigtless);
}

bool PhysicsComponent::isGrounded() const {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	return data->get<bool>(GROUNDED);
}

void PhysicsComponent::setGrounded(bool newWeigtless) {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	data->set(GROUNDED, newWeigtless);
}