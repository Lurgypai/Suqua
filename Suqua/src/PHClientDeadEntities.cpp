#include "PHClientDeadEntities.h"
#include "Game.h"
#include "EntityBaseComponent.h"
#include "NetworkDataComponent.h"

using UUID = Suqua::UUID;

PHClientDeadEntities::PHClientDeadEntities(PacketId id_) :
	PacketHandler{ id_ }
{}

void PHClientDeadEntities::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
    PacketId packet;
    data >> packet;

    while(data.hasMoreData()) {
        UUID uuid;
        data >> uuid;

        EntityId id = NetworkDataComponent::GetEntityId(uuid);
        if(id == 0) continue;

        auto* base = EntitySystem::GetComp<EntityBaseComponent>(id);
        if(base) base->isDead = true;
    }
}
