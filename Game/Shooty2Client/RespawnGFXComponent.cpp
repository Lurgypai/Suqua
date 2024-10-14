#include "RespawnGFXComponent.h"
#include "../Shooty2Core/RespawnComponent.h"
#include "HealthComponent.h"
#include "CharacterGFXComponent.h"

RespawnGFXComponent::RespawnGFXComponent(EntityId id_) : id{id_} {}

void RespawnGFXComponent::update() {
    auto* respawnComp = EntitySystem::GetComp<RespawnComponent>(id);
    auto* healthComp = EntitySystem::GetComp<HealthComponent>(id);
    if(healthComp->getHealth() > 0) return;
    auto* gfxComp = EntitySystem::GetComp<CharacterGFXComponent>(id);
    if(respawnComp->getTick() == 0) gfxComp->playAnimation("dead", false);
    else if (respawnComp->getTick() == respawnComp->getRespawnDelay()) gfxComp->stopAnimation();
}
