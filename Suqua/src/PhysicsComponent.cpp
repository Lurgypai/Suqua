#include "PhysicsComponent.h"
#include "AngleUtil.h"
#include "NetworkDataComponent.h"
#include "PhysicsData.h"
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
	}
}

EntityId PhysicsComponent::getId() const {
	return id;
}

const AABB & PhysicsComponent::getCollider() const {
	return collider;
}

void PhysicsComponent::reevaluatePosition() {
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
