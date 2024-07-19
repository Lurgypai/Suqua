#include <iostream>

#include "EntityBaseComponent.h"
#include "WackableComponent.h"
#include "PhysicsComponent.h"
#include "WackableSystem.h"
#include "HitboxComponent.h"
#include "DirectionComponent.h"
#include "Vec2.h"
#include "ParentComponent.h"
#include "EntityGenerator.h"
#include "PositionComponent.h"

void WackableSystem::update(Scene& scene) {
    if(!EntitySystem::Contains<WackableComponent>()) return;

    for(auto& wackableComp : EntitySystem::GetPool<WackableComponent>()) {
        auto otherBaseComp = EntitySystem::GetComp<EntityBaseComponent>(wackableComp.getId());
        if(otherBaseComp->isDead) continue;
        if(!otherBaseComp->isActive) continue;

        auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(wackableComp.getId());

        
        for(auto& hitComp : EntitySystem::GetPool<HitboxComponent>()) {
            //active
            auto baseComp = EntitySystem::GetComp<EntityBaseComponent>(hitComp.getId());
            if(!baseComp->isActive) continue;

            //intersects
            if(!hitComp.hitbox.intersects(physicsComp->getCollider())) continue;

            // since the entity is replaced with one that can't be hit, no worry
            //hasn't already been hit
            //if(!hitComp.canHitEntity(wackableComp.getId())) continue;

            //hitter has direction
            auto parentComp = EntitySystem::GetComp<ParentComponent>(hitComp.getId());
            auto dirComp = EntitySystem::GetComp<DirectionComponent>(parentComp->parentId);

            // hitComp.addHitEntity(wackableComp.getId());
            otherBaseComp->isDead = true;

            auto otherPhysicsComp = EntitySystem::GetComp<PhysicsComponent>(wackableComp.getId());
            EntityId ballId = EntityGenerator::SpawnBall(scene, otherPhysicsComp->position())[0];
            EntitySystem::MakeComps<RenderComponent>(1, &ballId);
            auto renderComp = EntitySystem::GetComp<RenderComponent>(ballId);
            renderComp->loadDrawable<Sprite>("bounce_ball");

            auto ballPhysicsComp = EntitySystem::GetComp<PhysicsComponent>(ballId);
            float horizVel = -200.f;
            Vec2f launchVel{horizVel, -100.f}; 
            if(dirComp->getCardinalDir() == 1) launchVel.x = -horizVel;

            ballPhysicsComp->setVel(launchVel);
        }
    }
}
