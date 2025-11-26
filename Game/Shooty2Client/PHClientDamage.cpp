#include <print>

#include "Game.h"
#include "PHClientDamage.h"
#include "UUID.h"
#include "NetworkDataComponent.h"
#include "HealthComponent.h"

PHClientDamage::PHClientDamage(PacketId id_) : PacketHandler(id_) {}

void PHClientDamage::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
    PacketId id;
    data >> id;

    while(data.hasMoreData()) {
        Suqua::UUID uuid;
        std::uint32_t damage;

        data >> uuid;
        data >> damage;

        EntityId entityId = NetworkDataComponent::GetEntityId(uuid);
        // this entity doesn't exist (already killed?)
        if(entityId == 0) continue;
        auto ndc = EntitySystem::GetComp<NetworkDataComponent>(entityId);

        auto healthComp = EntitySystem::GetComp<HealthComponent>(entityId);
        auto health = healthComp->health;
        healthComp->damage(damage);

        // update previous to prevent this from being sent out
        ndc->storePrev();
    }
}
