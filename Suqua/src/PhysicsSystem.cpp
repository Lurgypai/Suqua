#include "PhysicsSystem.h"
#include "PhysicsComponent.h"
#include "NetworkDataComponent.h"
#include "PositionComponent.h"
#include "EntityBaseComponent.h"
#include "NetworkOwnerComponent.h"

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
		auto baseComp = EntitySystem::GetComp<EntityBaseComponent>(entity);
		if (!baseComp->isActive) return;
		// auto networkOwnerComp = EntitySystem::GetComp<NetworkOwnerComponent>(entity);
		// if (networkOwnerComp != nullptr && networkOwnerComp->owner != NetworkOwnerComponent::Owner::local) return;

		PhysicsComponent* comp = EntitySystem::GetComp<PhysicsComponent>(entity);
		PositionComponent* posComp = EntitySystem::GetComp<PositionComponent>(entity);

		//refresh to make sure we're in the right place
		comp->refreshPos();

		if (!comp->isFrozen()) {
			//accelerate downwards, gravity
			if (!comp->isWeightless()) comp->accelerate({ 0, comp->getWeight()}); //hmmmmmmm


			//only the physics system manages grounded-ness, so this has to be a direct access
			comp->setGrounded(false);
			Vec2f currPos = posComp->getPos();
			Vec2f vel = comp->getVel();
			Vec2f newPos = { currPos.x + vel.x * static_cast<float>(timeDelta), currPos.y + vel.y * static_cast<float>(timeDelta) };
			
			if (comp->doesCollide()) {
				//handle collisions with the stage
				for (auto& otherComp : EntitySystem::GetPool<PhysicsComponent>()) {
                    auto otherBaseComp = EntitySystem::GetComp<EntityBaseComponent>(otherComp.getId());
                    if(!otherBaseComp->isActive) continue;
                    if(otherComp.id == comp->id) continue;
                    if(!otherComp.isCollidedWith()) continue;

                    auto& collider = otherComp.getCollider();
                    Vec2f res = comp->getRes();
                    //
                    //place we are updating too
                    AABB projection{ newPos, res };
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
                        if (vel.x < 0) comp->onCollide(CollisionDir::left);
                        else if (vel.x > 0) comp->onCollide(CollisionDir::right);
                    }
                    //vertical collision
                    else if (overlap.x == 0.0f && overlap.y != 0.0f) {
                        if (vel.y < 0) comp->onCollide(CollisionDir::up);
                        else if (vel.y > 0) comp->onCollide(CollisionDir::down);
                    }

                    //corner collision
                    else if (overlap.x != 0.0f && overlap.y != 0.0f) {
                        if (std::abs(vel.x) > std::abs(vel.y)) {
                            //this means don't resolve collisions allong the x axis
                            overlap.x = 0;

                            //and handle collision allong the y axis
                            if (vel.y < 0) comp->onCollide(CollisionDir::up);
                            else if (vel.y > 0) comp->onCollide(CollisionDir::down);
                        }
                        else {
                            overlap.y = 0;
                            if (vel.x < 0) comp->onCollide(CollisionDir::left);
                            else if (vel.x > 0) comp->onCollide(CollisionDir::right);
                        }
                    }
                    // comp->setVel(vel);
                    newPos -= overlap;
				}
			}

			currPos = newPos;
			posComp->setPos(currPos);
			comp->collider.pos = currPos;
		}
	}
}
