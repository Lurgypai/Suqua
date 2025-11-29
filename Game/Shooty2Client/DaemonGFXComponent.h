#pragma once
#include "ComponentMacros.h"
#include "ParticleEmitter.h"

class DaemonGFXComponent {
    CompMembers(DaemonGFXComponent);
public:
    DaemonGFXComponent(EntityId id_);
    void update(float timeDelta);
private:
    ParticleEmitter emitter;
};
