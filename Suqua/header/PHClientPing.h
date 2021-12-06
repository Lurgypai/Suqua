#pragma once
#include "PacketHandler.h"

class PHClientPing : public PacketHandler {
public:
	PHClientPing(PacketId id_);
	virtual void handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) override;
};