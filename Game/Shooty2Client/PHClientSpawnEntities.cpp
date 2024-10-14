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
        data >> pos.x;
        data >> pos.y;
        data >> netId;
        data >> owner;

        auto entities = EntitySpawnSystem::SpawnEntity(tag, *scene, pos, owner, false);
        // set the parent's net Id
        game.online.registerOnlineComponent(entities[0], netId);

        for(int i = 1; i < entities.size(); ++i) {
            //TODO real exception, we expect more entity desctiptions here
            if(!data.hasMoreData()) throw std::exception{}; 
            data >> netId;
            game.online.registerOnlineComponent(entities[i], netId);
        }
    }
}
