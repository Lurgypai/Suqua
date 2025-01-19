#include "PHServerState.h"
#include "Game.h"
#include "NetworkDataComponent.h"

PHServerState::PHServerState(PacketId id_) :
	PacketHandler{ id_ }
{}

void PHServerState::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
    PacketId packetId;
    data >> packetId;

    bool propogate;
    data >> propogate;

    UUID uuid;
    while(data.hasMoreData()) {
        data >> uuid;

        EntityId id = NetworkDataComponent::GetEntityId(uuid);
        if(id == 0) {
            NetworkDataComponent::MoveStreamPast(data);
            continue;
        }

        NetworkDataComponent* ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
        if(ndc) ndc->unserialize(data);
        else NetworkDataComponent::MoveStreamPast(data);
    }

    if(!propogate) return;
    data.setReadPos(0);
    for(PeerId& id : game.host.getConnectedPeers()) {
        if(id != sourcePeer) game.host.bufferDataToChannel(id, 0, data);
    }
}
