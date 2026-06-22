#include "EntityAddLiving.h"
#include "HurtboxComponent.h"
#include "ControllerComponent.h"
#include "HealthComponent.h"
#include "TopDownMoverComponent.h"
#include "NetworkDataComponent.h"
#include "DirectionComponent.h"
#include "NetworkDataComponentDataFields.h"
#include "EntityBaseComponent.h"

void EntityAddLiving(EntityId id, const EntityAddLivingArgs& args) {
    EntitySystem::MakeComps<TeamComponent>(1, &id, args.team);
    EntitySystem::MakeComps<HurtboxComponent>(1, &id,
            args.hurtboxOffset,
            args.hurtboxRes
            );
    EntitySystem::MakeComps<ControllerComponent>(1, &id);
    EntitySystem::MakeComps<HealthComponent>(1, &id, args.health);
    EntitySystem::MakeComps<TopDownMoverComponent>(1, &id, args.moveSpeed);

    auto* health = EntitySystem::GetComp<HealthComponent>(id);
    if(args.deathCallback == "none") health->deathCallback = nullptr;
    else if(args.deathCallback == "deactivate") {
        auto cb = [](EntityId id) {
            auto* base = EntitySystem::GetComp<EntityBaseComponent>(id);
            base->isActive = false;
        };
        health->deathCallback = cb;
    } else if (args.deathCallback != "default") throw std::runtime_error{std::format(
            "EntityAddLiving: Invalid deathCallback \"{}\"",
            args.deathCallback
            )};

    auto* ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
    if(ndc == nullptr) return;
    auto* dir = EntitySystem::GetComp<DirectionComponent>(id);
    ndc->set(HealthData::HEALTH, health->health);
    ndc->set(DirectionData::DIR, dir->dir);
}
