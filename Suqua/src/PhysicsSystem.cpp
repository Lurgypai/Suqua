#include "PhysicsSystem.h"
#include "PhysicsComponent.h"
#include "PositionComponent.h"

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
		PositionComponent * position = EntitySystem::GetComp<PositionComponent>(entity);

		//refresh to make sure we're in the right place
		comp->collider.pos = position->pos;

		if (!comp->frozen) {
			//accelerate downwards, gravity
			if (!comp->weightless)
				comp->accelerate({ 0, comp->weight });

			comp->grounded = false;
			Vec2f currPos = position->pos;
			Vec2f& vel = comp->vel;
			Vec2f newPos = { currPos.x + vel.x * static_cast<float>(timeDelta), currPos.y + vel.y * static_cast<float>(timeDelta) };

			//handle collisions with the stage
			for (auto& physicsComp : EntitySystem::GetPool<PhysicsComponent>()) {
				if (physicsComp.id != comp->id && physicsComp.collideable) {
					auto& collider = physicsComp.getCollider();
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
							if (vel.y > 0)
								comp->grounded = true;
							vel.y = 0.0f;
						}
						//corner collision
						else if (overlap.x != 0.0f && overlap.y != 0.0f) {
							float absXVel = abs(vel.x);
							float absYVel = abs(vel.y);
							if (absXVel > absYVel) {
								//this means don't resolve collisions allong the x axis
								overlap.x = 0;
								//and stop moving allong the y axis
								vel.y = 0;
								comp->grounded = true;
							}
							else {
								overlap.y = 0;
								vel.x = 0;
							}
						}
						newPos -= overlap;
					}
				}
			}
			currPos = newPos;
			position->pos = currPos;
			comp->collider.pos = currPos;
		}
	}
}