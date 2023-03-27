#pragma once
#include "PacketHandler.h"

class PHOOSPacket : public PacketHandler {
public:
	PHOOSPacket(PacketId id);
	virtual void handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) override;
};