#include "PHSyncState.h"
#include "SyncState.h"
#include "Game.h"

PHSyncState::PHSyncState(PeerId id_) : PacketHandler{ id_ } {}

void PHSyncState::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
	PacketId id;
	data >> id;

	game.sync.resyncStatePacket(data, game);
}