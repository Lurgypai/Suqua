#include "PHServerDeadEntities.h"
#include "Game.h"
#include "EntityBaseComponent.h"

PHServerDeadEntities::PHServerDeadEntities(PacketId id_) :
	PacketHandler{ id_ }
{}

void PHServerDeadEntities::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
    PacketId packet;
    data >> packet;

    while(data.hasMoreData()) {
        NetworkId netId;
        data >> netId;

        EntityId id = game.online.getEntity(netId);
        if(id == 0) continue;

        auto* base = EntitySystem::GetComp<EntityBaseComponent>(id);
        base->isDead = true;
    }

    data.setReadPos(0);
    for(const auto peer : game.host.getConnectedPeers()) {
        if(peer != sourcePeer) game.host.bufferDataToChannel(peer, 0, data);
    }
}
