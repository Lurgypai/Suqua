#include "PHSyncState.h"
#include "SyncState.h"
#include "Game.h"

PHSyncState::PHSyncState(PeerId id_) : PacketHandler{ id_ } {}

void PHSyncState::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
	PacketId id;
	data >> id;

	if (game.inputLagCompensationLevel > 1) {
		game.sync.resyncStatePacket(data, game);
	}
	else {
		SyncState s{ 0 };
		s.unserialize(data, game.online);
		s.applyState();
	}
}