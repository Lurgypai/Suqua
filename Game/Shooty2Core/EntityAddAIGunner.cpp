#include "EntityAddAIGunner.h"
#include "AIGunnerComponent.h"
#include "Shooty2NetworkDataFields.h"

void EntityAddAIGunner(EntityId id, const EntityAddAIGunnerArgs& args) {
	EntitySystem::MakeComps<AIGunnerComponent>(1, &id, args.followRadius, args.avoidRadius);
    auto* ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
	if (ndc == nullptr) return;

    auto* ai = EntitySystem::GetComp<AIGunnerComponent>(id);
    ndc->set(AIData::AI_STATE, reinterpret_cast<std::uint8_t&>(ai->state));
}