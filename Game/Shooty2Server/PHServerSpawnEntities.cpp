#include "PHServerSpawnEntities.h"
#include "Game.h"
#include "../Shooty2Core/EntitySpawnSystem.h"

#include "../Shooty2Core/Shooty2Packet.h"
#include <cstdint>

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

    while(data.hasMoreData()) {
        data >> tag;
        data >> pos;
        data >> uuidCount;

        std::vector<UUID> uuids{};
        uuids.reserve(uuidCount);
        for(int i = 0; i != uuidCount; ++i) {
            data >> uuid;
            uuids.push_back(uuid);
        }

        // spawn locally for ai and whatnot to know about
        auto entities = EntitySpawnSystem::SpawnEntity(tag, *scene, pos, NetworkDataComponent::Owner::foreign, uuids);

        game.networkEntityOwnershipSystem.addOwnedEntity(sourcePeer, tag, std::move(uuids));
    }

    data.setReadPos(0);
    for(PeerId& peerId : game.host.getConnectedPeers()) {
        if(peerId != sourcePeer) game.host.bufferDataToChannel(peerId, 0, data);
    }
}
