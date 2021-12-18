#include "PhysicsSystem.h"
#include "PhysicsComponent.h"
#include "NetworkDataComponent.h"
#include "PositionData.h"

#include <iostream>

using NDC = NetworkDataComponent;

PhysicsSystem::PhysicsSystem() {}

void PhysicsSystem::runPhysics(double timeDelta) {
	if (EntitySystem::Contains<PhysicsComponent>()) {
		for (auto& rawComp : EntitySystem::GetPool<PhysicsComponent>()) {
			runPhysics(timeDelta, rawComp.id);
		}
	}
}

void PhysicsSystem::runPhysics(double timeDelta, EntityId entity) {
	if (EntitySystem::Contains<PhysicsComponent>()) {
		PhysicsComponent * comp = EntitySystem::GetComp<PhysicsComponent>(entity);

		//refresh to make sure we're in the right place
		comp->refreshPos();

		if (!comp->isFrozen()) {
			//accelerate downwards, gravity
			if (!comp->isWeightless()) comp->accelerate({ 0, comp->getWeight()}); //hmmmmmmm


			//only the physics system manages grounded-ness, so this has to be a direct access
			comp->setGrounded(false);
			Vec2f currPos = comp->getPos();
			Vec2f vel = comp->getVel();
			Vec2f newPos = { currPos.x + vel.x * static_cast<float>(timeDelta), currPos.y + vel.y * static_cast<float>(timeDelta) };

			//handle collisions with the stage
			for (auto& otherComp : EntitySystem::GetPool<PhysicsComponent>()) {
				if (otherComp.id != comp->id && otherComp.isCollideable()) {
					auto& collider = otherComp.getCollider();
					Vec2f res = comp->getRes();
					//place we are updating too

					AABB projection{ newPos, res };

					//handle collisions
					if (collider.intersects(projection)) {
						Vec2f overlap{ 0, 0 };

						//moving down
						if (vel.y > 0) {
							//if we were above
							if (currPos.y + res.y <= collider.pos.y) {
								overlap.y = (newPos.y + res.y) - collider.pos.y;
							}
						}
						//moving up
						else if (vel.y < 0) {
							//below
							if (currPos.y >= collider.pos.y + collider.res.y) {
								overlap.y = newPos.y - (collider.pos.y + collider.res.y);
							}
						}

						//moving right
						if (vel.x > 0) {
							//if we were to the left of it
							if (currPos.x + res.x <= collider.pos.x) {
								overlap.x = (newPos.x + res.x) - collider.pos.x;
							}
						}
						//moving left
						else if (vel.x < 0) {
							if (currPos.x >= collider.pos.x + collider.res.x) {
								overlap.x = newPos.x - (collider.pos.x + collider.res.x);
							}
						}

						//horizontal collision
						if (overlap.x != 0.0f && overlap.y == 0.0f) {
							vel.x = 0.0f;
						}
						//vertical collision
						else if (overlap.x == 0.0f && overlap.y != 0.0f) {
							if (vel.y > 0) comp->setGrounded(true);
							vel.y = 0.0f;
						}
						//corner collision
						else if (overlap.x != 0.0f && overlap.y != 0.0f) {
							if (std::abs(vel.x) > std::abs(vel.y)) {
								//this means don't resolve collisions allong the x axis
								overlap.x = 0;
								//and stop moving along the y axis
								vel.y = 0;
								comp->setGrounded(true);
							}
							else {
								overlap.y = 0;
								vel.x = 0;
							}
						}
						comp->setVel(vel);
						newPos -= overlap;
					}
				}
			}
			currPos = newPos;
			comp->setPos(currPos);
			comp->collider.pos = currPos;
		}
	}
}