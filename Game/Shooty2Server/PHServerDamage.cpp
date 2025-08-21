#include "PHServerDamage.h"
#include "UUID.h"
#include "NetworkDataComponent.h"
#include "HealthComponent.h"

PHServerDamage::PHServerDamage(PacketId id_) : PacketHandler(id_) {}

void PHServerDamage::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
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

        // apply damage to local entities
        if(ndc->owner == NetworkDataComponent::Owner::local_shared) {
            auto healthComp = EntitySystem::GetComp<HealthComponent>(entityId);
            healthComp->damage(damage);
            continue;
        }

        // may need to at some point notify local entities when they hit something
    }
}
