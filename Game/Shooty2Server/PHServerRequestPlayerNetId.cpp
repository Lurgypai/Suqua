#include "PHServerRequestPlayerNetId.h"
#include "Game.h"

#include "../Shooty2Core/Shooty2Packet.h"

PHServerRequestPlayerNetId::PHServerRequestPlayerNetId(PacketId id_) :
	PacketHandler{ id_ }
{}

void PHServerRequestPlayerNetId::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
    std::cout << "Received request for a network ID from peer " << sourcePeer << "...\n";
    auto netId = game.online.getFreeNetworkId();
    data << netId;

    ByteStream makePuppet;
    makePuppet << Shooty2Packet::MakePlayerPuppet;
    makePuppet << netId;

    for(PeerId& peerId : game.host.getConnectedPeers()) {
        if(peerId != sourcePeer) {
            game.host.bufferDataToChannel(peerId, 0, makePuppet);
        }
        else {
            game.host.bufferDataToChannel(peerId, 0, data);
        }
    }
}
