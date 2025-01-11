#include "CHKill.h"
#include "PhysicsComponent.h"
#include "EntityBaseComponent.h"

CHKill::~CHKill() {};

void CHKill::onCollide(PhysicsComponent& physicsComp, CollisionDir dir) {
    auto base = EntitySystem::GetComp<EntityBaseComponent>(physicsComp.getId());
    base->isDead = true;
}
