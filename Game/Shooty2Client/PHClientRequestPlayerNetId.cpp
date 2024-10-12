#include "PHClientRequestPlayerNetId.h"
#include "Game.h"

#include "../Shooty2Core/Shooty2Packet.h"

PHClientRequestPlayerNetId::PHClientRequestPlayerNetId(PacketId id_, EntityId playerId_) :
	PacketHandler{ id_ },
    playerId{playerId_}
{}

void PHClientRequestPlayerNetId::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
    std::cout << "Client received approval of Network Id request.\n";
    PacketId packetId;
    data >> packetId;

    NetworkId netId;
    data >> netId;

    game.online.registerOnlineComponent(playerId, netId);
}
