#include "PHClientSpawnEntities.h"
#include "Game.h"

#include "../Shooty2Core/Shooty2Packet.h"
#include "../Shooty2Core/EntitySpawnSystem.h"
#include <cstdint>

using UUID = Suqua::UUID;

PHClientSpawnEntities::PHClientSpawnEntities(PacketId id_, Scene* scene_) :
	PacketHandler{ id_ },
    scene{scene_}
{}

void PHClientSpawnEntities::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
    PacketId packetId;
    data >> packetId;

    std::string tag;
    Vec2f pos;
    std::uint32_t uuidCount;
    UUID uuid;

    while(data.hasMoreData()) {
        data >> tag;
        data >> pos;
        data >> uuid;

        EntitySpawnSystem::SpawnEntity(tag, *scene, pos, NetworkDataComponent::Owner::foreign, uuid);
    }
}
