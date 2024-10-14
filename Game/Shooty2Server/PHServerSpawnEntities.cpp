#include "PHServerSpawnEntities.h"
#include "Game.h"

#include "../Shooty2Core/Shooty2Packet.h"
#include "NetworkOwnerComponent.h"
#include <exception>

PHServerSpawnEntities::PHServerSpawnEntities(PacketId id_) :
	PacketHandler{ id_ }
{}

void PHServerSpawnEntities::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {

    PacketId id;
    data >> id;

    std::string tag;
    EntityId targetEntity;
    Vec2f pos;
    uint32_t subCount;

    ByteStream assignNetId;
    assignNetId << Shooty2Packet::AssignNetworkId;

    ByteStream spawn;
    spawn << Shooty2Packet::SpawnEntities;

    std::vector<NetworkId> assignedNetIds{};
    while(data.hasMoreData()) {
        assignedNetIds.clear();

        // read the parent entity
        data >> tag;
        data >> targetEntity;
        data >> pos.x;
        data >> pos.y;

        // assign net id to parent
        assignNetId << targetEntity;
        assignedNetIds.push_back(game.online.getFreeNetworkId());
        assignNetId << assignedNetIds.back();

        // prepare spawn packet for parent
        spawn << tag;
        spawn << pos.x;
        spawn << pos.y;
        spawn << assignedNetIds.back();
        spawn << NetworkOwnerComponent::Owner::foreign;

        data >> subCount;
        // should crash out if subcount is bad
        for(int i = 0; i != subCount; ++i) {
            if(!data.hasMoreData()) throw std::exception{};
            // assign netID for child
            data >> targetEntity;
            assignNetId << targetEntity;
            assignedNetIds.push_back(game.online.getFreeNetworkId());
            assignNetId << assignedNetIds.back();

            // prepare spawn packet for child
            spawn << assignedNetIds.back();
        }

        game.networkEntityOwnershipSystem.addOwnedEntity(sourcePeer, tag, std::move(assignedNetIds));
    }

    for(PeerId& peerId : game.host.getConnectedPeers()) {
        if(peerId != sourcePeer) {
            game.host.bufferDataToChannel(peerId, 0, spawn);
        }
        else {
            game.host.bufferDataToChannel(peerId, 0, assignNetId);
        }
    }
}
