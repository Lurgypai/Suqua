#include "PHServerState.h"
#include "Game.h"

PHServerState::PHServerState(PacketId id_) :
	PacketHandler{ id_ }
{}

void PHServerState::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
    for(PeerId& id : game.host.getConnectedPeers()) {
        if(id != sourcePeer) game.host.bufferDataToChannel(id, 0, data);
    }
}
