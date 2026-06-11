#include "PhysicsSystem.h"
#include "PhysicsComponent.h"
#include "NetworkDataComponent.h"
#include "PositionComponent.h"
#include "EntityBaseComponent.h"

using NDC = NetworkDataComponent;

PhysicsSystem::PhysicsSystem() {}

void PhysicsSystem::getActive() {
    if(!EntitySystem::Contains<PhysicsComponent>()) return;

    active.clear();
    collidesWith.clear();
    for(auto& physicsComp : EntitySystem::GetPool<PhysicsComponent>()) {
        auto* base = EntitySystem::GetComp<EntityBaseComponent>(physicsComp.getId());
        if(base->isDead || !base->isActive) continue;
        active.push_back(&physicsComp);
        if(!physicsComp.collidesWith) continue;
        collidesWith.push_back(&physicsComp);
    }
};

void PhysicsSystem::getActiveOwned() {
    if(!EntitySystem::Contains<PhysicsComponent>()) return;

    active.clear();
    collidesWith.clear();
    for(auto& physicsComp : EntitySystem::GetPool<PhysicsComponent>()) {
        auto* ndc = EntitySystem::GetComp<NDC>(physicsComp.getId());
        if(ndc == nullptr) continue;

        if(ndc->owner == NDC::Owner::foreign) {
            // skip the position component call
            physicsComp.refreshPos();
            continue;
        }

        auto* base = EntitySystem::GetComp<EntityBaseComponent>(physicsComp.getId());
        if(base->isDead || !base->isActive) continue;
        active.push_back(&physicsComp);
        if(!physicsComp.isCollidedWith()) continue;
        collidesWith.push_back(&physicsComp);
    }
}

void PhysicsSystem::runPhysics(double timeDelta) {
    getActive();
    for (auto physicsComp : active) {
        runPhysics(timeDelta, *physicsComp);
    }
}

void PhysicsSystem::runPhysicsOnOwned(double timeDelta) {
    getActiveOwned();
    for (auto physicsComp : active) {
        runPhysics(timeDelta, *physicsComp);
    }
}

void PhysicsSystem::runPhysics(double timeDelta, PhysicsComponent& physicsComp) {
	if (EntitySystem::Contains<PhysicsComponent>()) {
		PositionComponent* posComp = EntitySystem::GetComp<PositionComponent>(physicsComp.getId());

		//refresh to make sure we're in the right place
		physicsComp.refreshPos();

		if (!physicsComp.isFrozen()) {
			//accelerate downwards, gravity
			if (!physicsComp.isWeightless()) physicsComp.accelerate({ 0, physicsComp.getWeight()}); //hmmmmmmm
			//only the physics system manages grounded-ness, so this has to be a direct access
			physicsComp.setGrounded(false);

            // set by reset pos
			Vec2f currPos = physicsComp.collider.pos;
			const Vec2f& vel = physicsComp.vel;
			Vec2f newPos = { currPos.x + vel.x * static_cast<float>(timeDelta), currPos.y + vel.y * static_cast<float>(timeDelta) };
			
			if (physicsComp.doesCollide()) {
                const Vec2f& res = physicsComp.getRes();
                const AABB projection{ newPos, res };

				//handle collisions with other entities
				for (auto otherComp : collidesWith) {
                    if(otherComp->id == physicsComp.id) continue;

                    auto& collider = otherComp->getCollider();

                    //place we are updating too
                    if(!collider.intersects(projection)) continue;

                    //handle collisions
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
                        if (vel.x < 0) physicsComp.onCollide(CollisionDir::left);
                        else if (vel.x > 0) physicsComp.onCollide(CollisionDir::right);
                    }
                    //vertical collision
                    else if (overlap.x == 0.0f && overlap.y != 0.0f) {
                        if (vel.y < 0) physicsComp.onCollide(CollisionDir::up);
                        else if (vel.y > 0) physicsComp.onCollide(CollisionDir::down);
                    }

                    //corner collision
                    else if (overlap.x != 0.0f && overlap.y != 0.0f) {
                        if (std::abs(vel.x) > std::abs(vel.y)) {
                            //this means don't resolve collisions allong the x axis
                            overlap.x = 0;

                            //and handle collision allong the y axis
                            if (vel.y < 0) physicsComp.onCollide(CollisionDir::up);
                            else if (vel.y > 0) physicsComp.onCollide(CollisionDir::down);
                        }
                        else {
                            overlap.y = 0;
                            if (vel.x < 0) physicsComp.onCollide(CollisionDir::left);
                            else if (vel.x > 0) physicsComp.onCollide(CollisionDir::right);
                        }
                    }
                    // physicsComp.setVel(vel);
                    newPos -= overlap;
				}

                // handle collisions with stage
                // codes gonna be similar to above,
                // but you'll have to calculate tile position from the tilemap
                // by checking if there are any tiles along the side being checked
                // IE moving right, check if their are any tiles along the right edge
			}

			currPos = newPos;
            posComp->pos = currPos;
			physicsComp.collider.pos = currPos;
		}
	}
}
