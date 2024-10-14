#include "PHClientAssignNetworkId.h"
#include "Game.h"

#include "../Shooty2Core/Shooty2Packet.h"

PHClientAssignNetworkId::PHClientAssignNetworkId(PacketId id_) :
	PacketHandler{ id_ }
{}

void PHClientAssignNetworkId::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
    std::cout << "Client received approval of Network Id request." << std::endl;
    PacketId packetId;
    data >> packetId;

    EntityId entityId;
    NetworkId netId;
    while(data.hasMoreData()) {
        data >> entityId;
        data >> netId;
        game.online.registerOnlineComponent(entityId, netId);
    }
}
