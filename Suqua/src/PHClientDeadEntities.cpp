#include "PHClientDeadEntities.h"
#include "Game.h"
#include "EntityBaseComponent.h"

PHClientDeadEntities::PHClientDeadEntities(PacketId id_) :
	PacketHandler{ id_ }
{}

void PHClientDeadEntities::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
    PacketId packet;
    data >> packet;

    while(data.hasMoreData()) {
        NetworkId netId;
        data >> netId;

        EntityId id = game.online.getEntity(netId);
        if(id == 0) continue;

        auto* base = EntitySystem::GetComp<EntityBaseComponent>(id);
        if(base) base->isDead = true;

        game.online.freeNetId(netId);
    }
}
