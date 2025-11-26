#include "TeleportZoneGFXComponent.h"
#include "EntityBaseComponent.h"
#include "RandomUtil.h"
#include "PhysicsComponent.h"
#include "GLRenderer.h"

TeleportZoneGFXComponent::TeleportZoneGFXComponent(EntityId id_) :
    id{id_},
    particleSpawnDelay{20},
    particleSpawnElapsed{0}
{}

void TeleportZoneGFXComponent::update(int timeDelta) {
    particleSpawnElapsed += timeDelta;

    if(particleSpawnElapsed < particleSpawnDelay) return;

    particleSpawnElapsed -= particleSpawnDelay;

    auto exitBase = EntitySystem::GetComp<EntityBaseComponent>(id);
    if(exitBase->isActive) {
        auto exitPhysics = EntitySystem::GetComp<PhysicsComponent>(id);
        auto pos = exitPhysics->position();
        float angle = randFloat(0.f, 3.1415926535898f * 2.f);
        Vec2f offset{ 50.f, 0.f };
        offset.angle(angle);
        pos += offset;
        Particle base{
            Color{ 1.f, 1.f, 1.f, 1.f },
            pos,
            -90,
            .2f,
            100,
            0
        };
        GLRenderer::SpawnParticles("exit", 1, base);
    }
}
