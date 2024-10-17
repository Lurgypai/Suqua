#include "PHServerSpawnEntities.h"
#include "Game.h"
#include "../Shooty2Core/EntitySpawnSystem.h"

#include "../Shooty2Core/Shooty2Packet.h"
#include "NetworkOwnerComponent.h"
#include <exception>

PHServerSpawnEntities::PHServerSpawnEntities(PacketId id_, Scene* scene_) :
	PacketHandler{ id_ },
    scene{scene_}
{}

void PHServerSpawnEntities::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {

    PacketId id;
    data >> id;

    std::string tag;
    EntityId targetEntity;
    Vec2f pos;

    ByteStream assignNetId;
    assignNetId << Shooty2Packet::AssignNetworkId;

    ByteStream spawn;
    spawn << Shooty2Packet::SpawnEntities;

    std::vector<NetworkId> assignedNetIds{};
    while(data.hasMoreData()) {
        assignedNetIds.clear();

        // read the parent entity
        data >> tag;
        data >> pos;

        // spawn locally for ai and whatnot to know about
        auto entities = EntitySpawnSystem::SpawnEntity(tag, *scene, pos, NetworkOwnerComponent::Owner::foreign, false);
        for(auto& entity : entities) {
            game.online.addOnlineComponent(entity);
            auto* netComp = EntitySystem::GetComp<OnlineComponent>(entity);
            assignedNetIds.push_back(netComp->getNetId());
        }

        // prepare spawn packet
        spawn << tag;
        spawn << pos;
        spawn << NetworkOwnerComponent::Owner::foreign;

        // should crash out if subcount is bad
        for(const auto& netId : assignedNetIds) {
            if(!data.hasMoreData()) throw std::exception{};
            data >> targetEntity;

            assignNetId << targetEntity;
            assignNetId << netId;

            // prepare spawn packet for child
            spawn << netId;
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
