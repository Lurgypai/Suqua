#include "PHServerAddPlayer.h"
#include "Game.h"
#include "UUID.h"

using Suqua::UUID;

PHServerAddPlayer::PHServerAddPlayer(PacketId id_) : PacketHandler(id_) {}

void PHServerAddPlayer::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
    PacketId id;
    data >> id;

    UUID uuid;
    data >> uuid;

    game.networkEntityOwnershipSystem.addPlayer(sourcePeer, uuid);
}
