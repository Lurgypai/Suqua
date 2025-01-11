#pragma once
#include "PacketHandler.h"

class PHServerState : public PacketHandler {
public:
	PHServerState(PacketId id_);
	virtual void handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) override;
};
