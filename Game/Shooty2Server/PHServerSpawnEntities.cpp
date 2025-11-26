#include "PHServerSpawnEntities.h"
#include "Game.h"
#include "EntitySpawnSystem.h"

#include "../Shooty2Core/Shooty2Packet.h"
#include <cstdint>

using UUID = Suqua::UUID;

PHServerSpawnEntities::PHServerSpawnEntities(PacketId id_, Scene* scene_) :
	PacketHandler{ id_ },
    scene{scene_}
{}

void PHServerSpawnEntities::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {

    PacketId id;
    data >> id;

    std::string tag;
    Vec2f pos;
    std::uint32_t uuidCount;
    UUID uuid;

    // spawn all locally
    while(data.hasMoreData()) {
        data >> tag;
        data >> pos;
        data >> uuid;
        // spawn locally for ai and whatnot to know about
        auto entity = EntitySpawnSystem::SpawnEntity(tag, *scene, pos, NetworkDataComponent::Owner::foreign, uuid);

        game.networkEntityOwnershipSystem.addOwnedEntity(sourcePeer, tag, uuid);
    }

    // send to clients
    data.setPos(0);
    for(PeerId& peerId : game.host.getConnectedPeers()) {
        if(peerId != sourcePeer) game.host.bufferDataToChannel(peerId, 0, data);
    }
}
