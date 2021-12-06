#pragma once
#include "PacketHandler.h"

class PHServerPing : public PacketHandler {
public:
	PHServerPing(PacketId id_);
	virtual void handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) override;
};