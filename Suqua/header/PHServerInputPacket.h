#pragma once
#include "PacketHandler.h"

class PHServerInputPacket : public PacketHandler {
public:
	PHServerInputPacket(PacketId id);
	virtual void handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) override;
};