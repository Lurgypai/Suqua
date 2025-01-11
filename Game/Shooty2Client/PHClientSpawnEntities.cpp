#include "PHClientSpawnEntities.h"
#include "Game.h"

#include "../Shooty2Core/Shooty2Packet.h"
#include "../Shooty2Core/EntitySpawnSystem.h"

PHClientSpawnEntities::PHClientSpawnEntities(PacketId id_, Scene* scene_) :
	PacketHandler{ id_ },
    scene{scene_}
{}

void PHClientSpawnEntities::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
    std::cout << "Client received spawn entities command\n";
    PacketId packetId;
    data >> packetId;

    std::string tag;
    NetworkId netId;
    NetworkOwnerComponent::Owner owner;
    Vec2f pos;

    while(data.hasMoreData()) {
        data >> tag;
        data >> pos;
        data >> owner;

        auto entities = EntitySpawnSystem::SpawnEntity(tag, *scene, pos, owner, false);
        // set the parent's net Id

        for(const auto& entity : entities) {
            if(!data.hasMoreData()) throw std::exception{}; 
            data >> netId;
            game.online.registerOnlineComponent(entity, netId);
        }
    }
}
