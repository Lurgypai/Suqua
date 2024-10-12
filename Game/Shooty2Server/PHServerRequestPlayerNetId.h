#pragma once
#include "PacketHandler.h"

class PHServerRequestPlayerNetId : public PacketHandler {
public:
	PHServerRequestPlayerNetId(PacketId id_);
	virtual void handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) override;
};
