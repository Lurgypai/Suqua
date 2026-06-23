#include "EntityAddBullet.h"
#include "HitboxComponent.h"
#include "DamageComponent.h"
#include "BasicDamageCalculator.h"
#include "PhysicsComponent.h"
#include "LifeTimeComponent.h"
#include "CHKill.h"

void EntityAddBullet(EntityId id, const EntityAddBulletArgs& args) {
    EntitySystem::MakeComps<TeamComponent>(1, &id, args.team);
    EntitySystem::MakeComps<HitboxComponent>(1, &id, Vec2f{0, 0}, args.res, true);
    EntitySystem::MakeComps<DamageComponent>(1, &id);
    if (args.lifetime > 0) EntitySystem::MakeComps<LifeTimeComponent>(1, &id, args.lifetime);

    auto* damageComp = EntitySystem::GetComp<DamageComponent>(id);
    damageComp->setDamageCalculator<BasicDamageCalculator>(args.damage);

    auto* physics = EntitySystem::GetComp<PhysicsComponent>(id);
    if(physics == nullptr) throw std::runtime_error{
            "EntityAddBullet: Missing physics component."
            };
    if(args.collisionHandler == "CHKill") physics->loadCollisionHandler<CHKill>();

}
