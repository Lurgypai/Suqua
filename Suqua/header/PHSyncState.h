#pragma once
#include "PacketHandler.h"

class PHSyncState : public PacketHandler {
public:
	PHSyncState(PeerId id_);
	virtual void handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) override;
};